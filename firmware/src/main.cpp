#include <Arduino.h>
#include "config.h"
#include "gains.h"

#define EX_MAX 0.10f

enum Estado : uint8_t { INACTIVO, HOMING, LISTO, BALANCE, FALLA };
static const char *NOMBRE[] = {"INACTIVO", "HOMING", "LISTO", "BALANCE", "FALLA"};

HardwareSerial usb(PIN_RX, PIN_TX);
HardwareSerial esp(PIN_ESP_RX, PIN_ESP_TX);

struct Puerto {
  HardwareSerial &s;
  uint8_t div;
  bool telem;
  char buf[64];
  uint8_t n;
};
Puerto puertos[] = {{usb, TELEM_DIV, false, {}, 0}, {esp, TELEM_DIV_ESP, false, {}, 0}};

class Ambos : public Print {
 public:
  size_t write(uint8_t c) override {
    for (auto &p : puertos) p.s.write(c);
    return 1;
  }
  size_t write(const uint8_t *b, size_t n) override {
    for (auto &p : puertos) p.s.write(b, n);
    return n;
  }
} ser;
HardwareTimer *tStep, *tCtl;

volatile int32_t sps = 0;
volatile int32_t posPasos = 0;
volatile bool tickCtl = false;

static const float TS = 1.0f / CTL_HZ;
static Estado est = INACTIVO;
static bool conCero = false, ondaCuad = false;
static float K[4] = {K_LQR[0], K_LQR[1], K_LQR[2], K_LQR[3]};
static uint8_t fHoming = 0;
static float v = 0, a = 0, xr = 0, w = 0, th = 0, x = 0;
static int32_t encTot = 0;
static int32_t hist[N_VEL + 1];
static uint8_t hIdx = 0;
static uint32_t tArmar = 0, nCtl = 0, tRef0 = 0;

extern "C" void SystemClock_Config(void) {
  RCC_OscInitTypeDef osc = {};
  RCC_ClkInitTypeDef clk = {};
  osc.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  osc.HSEState = RCC_HSE_ON;
  osc.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  osc.PLL.PLLState = RCC_PLL_ON;
  osc.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  osc.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&osc) != HAL_OK) Error_Handler();
  clk.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  clk.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  clk.AHBCLKDivider = RCC_SYSCLK_DIV1;
  clk.APB1CLKDivider = RCC_HCLK_DIV2;
  clk.APB2CLKDivider = RCC_HCLK_DIV1;
  if (HAL_RCC_ClockConfig(&clk, FLASH_LATENCY_2) != HAL_OK) Error_Handler();
}

void isrPaso() {
  static bool alto = false;
  static int8_t dir = 0;
  static uint32_t acc = 0;
  bool eraAlto = alto;
  if (alto) { digitalWriteFast(PN_STEP, LOW); alto = false; }
  int32_t s = sps;
  if (s == 0) { acc = 0; return; }
  int8_t d = s > 0 ? 1 : -1;
  if (d != dir) {
    dir = d;
    digitalWriteFast(PN_DIR, (d * MOTOR_SIGNO) > 0 ? HIGH : LOW);
    return;
  }
  acc += (uint32_t)(s > 0 ? s : -s);
  if (acc >= STEP_TICK_HZ && !eraAlto) {
    acc -= STEP_TICK_HZ;
    digitalWriteFast(PN_STEP, HIGH);
    alto = true;
    posPasos += d;
  }
  if (acc > 2 * STEP_TICK_HZ) acc = 2 * STEP_TICK_HZ;
}

void isrCtl() { tickCtl = true; }

void encInit() {
  __HAL_RCC_TIM3_CLK_ENABLE();
  pinMode(PIN_ENC_A, INPUT_PULLUP);
  pinMode(PIN_ENC_B, INPUT_PULLUP);
  TIM3->CR1 = 0;
  TIM3->SMCR = TIM_SMCR_SMS_0 | TIM_SMCR_SMS_1;
  TIM3->CCMR1 = TIM_CCMR1_CC1S_0 | TIM_CCMR1_CC2S_0 | (0x9 << TIM_CCMR1_IC1F_Pos) | (0x9 << TIM_CCMR1_IC2F_Pos);
  TIM3->CCER = 0;
  TIM3->ARR = 0xFFFF;
  TIM3->CNT = 0;
  TIM3->CR1 = TIM_CR1_CEN;
}

int32_t encLeer() {
  static uint16_t ult = 0;
  uint16_t c = TIM3->CNT;
  encTot += (int16_t)(c - ult);
  ult = c;
  return ENC_SIGNO * encTot;
}

void encCero() {
  encLeer();
  encTot = 0;
  for (auto &h : hist) h = 0;
}

float envolver(float ang) {
  while (ang > PI) ang -= TWO_PI;
  while (ang <= -PI) ang += TWO_PI;
  return ang;
}

bool limIzq() { return digitalRead(PIN_LIM_IZQ) == LIM_ACTIVO; }
bool limDer() { return digitalRead(PIN_LIM_DER) == LIM_ACTIVO; }

void motor(bool on) { digitalWrite(PIN_EN, on ? LOW : HIGH); }

void fijarPos(float xm) {
  noInterrupts();
  posPasos = (int32_t)lroundf(xm * PASOS_M);
  interrupts();
}

void cambiar(Estado e) {
  if (e == est) return;
  est = e;
  if (e == INACTIVO || e == FALLA) { v = 0; a = 0; sps = 0; motor(false); conCero = false; }
  if (e == BALANCE) { a = 0; tRef0 = millis(); }
  ser.print("# estado: ");
  ser.println(NOMBRE[e]);
}

float frenar(float vel) {
  float dv = A_FRENO * TS;
  if (vel > dv) return vel - dv;
  if (vel < -dv) return vel + dv;
  return 0;
}

void telemetria(Print &o) {
  o.print(millis()); o.print(',');
  o.print(x, 4);     o.print(',');
  o.print(v, 3);     o.print(',');
  o.print(th, 4);    o.print(',');
  o.print(w, 3);     o.print(',');
  o.print(a, 2);     o.print(',');
  o.print(xr, 3);    o.print(',');
  o.println((int)est);
}

void control() {
  int32_t c = encLeer();
  th = envolver(c * (TWO_PI / ENC_CPR) + PI);
  hIdx = (hIdx + 1) % (N_VEL + 1);
  hist[hIdx] = c;
  int32_t cViejo = hist[(hIdx + 1) % (N_VEL + 1)];
  float wRaw = (c - cViejo) * (TWO_PI / ENC_CPR) / (N_VEL * TS);
  w += (TS / (TS + TAU_W)) * (wRaw - w);
  x = posPasos / PASOS_M;

  bool li = limIzq(), ld = limDer();
  if (est != INACTIVO && est != FALLA && est != HOMING && (li || ld)) {
    ser.println("# final de carrera activado");
    cambiar(FALLA);
  }

  switch (est) {
    case HOMING:
      if (ld) { cambiar(FALLA); break; }
      if (fHoming == 0) {
        v = -V_HOMING;
        if (li) { v = 0; fijarPos(X_SWITCH_IZQ); fHoming = 1; }
      } else {
        v = x < 0 ? V_HOMING : 0;
        if (x >= 0) { v = 0; conCero = true; cambiar(LISTO); }
      }
      break;

    case LISTO:
      v = frenar(v);
      if (fabsf(th) < ANG_ARMAR && v == 0) {
        if (!tArmar) tArmar = millis();
        if (millis() - tArmar > T_ARMAR_MS) cambiar(BALANCE);
      } else {
        tArmar = 0;
      }
      break;

    case BALANCE: {
      if (fabsf(th) > ANG_CAIDA || fabsf(x) > X_SUAVE) {
        ser.println(fabsf(th) > ANG_CAIDA ? "# péndulo caído" : "# límite de software");
        tArmar = 0;
        cambiar(LISTO);
        break;
      }
      if (ondaCuad) xr = ((millis() - tRef0) % (uint32_t)(REF_T * 1000) < REF_T * 500) ? REF_AMP : -REF_AMP;
      float ex = constrain(x - xr, -EX_MAX, EX_MAX);
      a = -(K[0] * ex + K[1] * v + K[2] * th + K[3] * w);
      a = constrain(a, -A_MAX, A_MAX);
      v = constrain(v + a * TS, -V_MAX, V_MAX);
      break;
    }

    default:
      v = 0;
      break;
  }

  sps = (int32_t)lroundf(v * PASOS_M);

  ++nCtl;
  for (auto &p : puertos)
    if (p.telem && nCtl % p.div == 0) telemetria(p.s);
}

void ayuda() {
  ser.println(F("# Comandos:"));
  ser.println(F("#  h  homing          z  x=0 aquí (sin homing)"));
  ser.println(F("#  e  habilitar        d  deshabilitar"));
  ser.println(F("#  c  encoder: péndulo colgando = abajo"));
  ser.println(F("#  w  onda cuadrada on/off   r<m>  referencia (ej. r0.05)"));
  ser.println(F("#  t  telemetría on/off (t1/t0)  i  info   ?  ayuda"));
  ser.println(F("#  k<k1>,<k2>,<k3>,<k4>  ganancias LQR en vivo"));
  ser.println(F("# telemetría: t_ms,x,v,theta,omega,a,x_ref,estado"));
}

void infoK() {
  ser.print("#K=");
  for (int i = 0; i < 4; i++) { ser.print(K[i], 4); ser.print(i < 3 ? ',' : '\n'); }
}

bool leerK(char *q) {
  float nk[4];
  for (int i = 0; i < 4; i++) {
    char *e;
    nk[i] = strtof(q, &e);
    if (e == q || !isfinite(nk[i])) return false;
    q = (*e == ',') ? e + 1 : e;
  }
  memcpy(K, nk, sizeof(K));
  return true;
}

void info() {
  ser.print("# est="); ser.print(NOMBRE[est]);
  ser.print(" x="); ser.print(x, 4);
  ser.print(" th[deg]="); ser.print(th * RAD_TO_DEG, 2);
  ser.print(" limI="); ser.print(limIzq());
  ser.print(" limD="); ser.print(limDer());
  ser.print(" cero="); ser.println(conCero);
  infoK();
}

void comando(char *l, Puerto &p) {
  switch (l[0]) {
    case 'h':
      if (!USAR_HOMING) { ser.println("# homing desactivado"); break; }
      fHoming = 0; motor(true); cambiar(HOMING); break;
    case 'z': fijarPos(0); conCero = true; ser.println("# x = 0"); break;
    case 'e':
      if (!conCero) { ser.println("# primero h o z"); break; }
      if (limIzq() || limDer()) { ser.println("# final de carrera activo"); break; }
      motor(true); cambiar(LISTO); break;
    case 'd': cambiar(INACTIVO); break;
    case 'c': encCero(); ser.println("# encoder en cero (abajo)"); break;
    case 'w': ondaCuad = !ondaCuad; if (!ondaCuad) xr = 0; tRef0 = millis(); break;
    case 'r': xr = constrain(atof(l + 1), -X_SUAVE, X_SUAVE); ondaCuad = false; break;
    case 't':
      p.telem = l[1] ? l[1] == '1' : !p.telem;
      if (p.telem) p.s.println("#t_ms,x,v,theta,omega,a,x_ref,estado");
      break;
    case 'k':
      if (!leerK(l + 1)) ser.println("# formato: k<k1>,<k2>,<k3>,<k4>");
      infoK();
      break;
    case 'i': info(); break;
    case '?': ayuda(); break;
  }
}

void leerSerie() {
  for (auto &p : puertos) {
    while (p.s.available()) {
      char ch = p.s.read();
      if (ch == '\n' || ch == '\r') {
        if (p.n) { p.buf[p.n] = 0; comando(p.buf, p); p.n = 0; }
      } else if (p.n < sizeof(p.buf) - 1) {
        p.buf[p.n++] = ch;
      }
    }
  }
}

void setup() {
  for (uint32_t p : {PIN_HOTEND, PIN_CAMA, PIN_FAN}) { pinMode(p, OUTPUT); digitalWrite(p, LOW); }
  pinMode(PIN_EN, OUTPUT);
  motor(false);
  pinMode(PIN_STEP, OUTPUT);
  pinMode(PIN_DIR, OUTPUT);
  pinMode(PIN_LIM_IZQ, INPUT_PULLUP);
  pinMode(PIN_LIM_DER, INPUT_PULLUP);

  usb.begin(BAUD);
  esp.begin(BAUD_ESP);
  encInit();
  encCero();

  tStep = new HardwareTimer(TIM2);
  tStep->setOverflow(STEP_TICK_HZ, HERTZ_FORMAT);
  tStep->attachInterrupt(isrPaso);
  tStep->setInterruptPriority(0, 0);
  tStep->resume();

  tCtl = new HardwareTimer(TIM4);
  tCtl->setOverflow(CTL_HZ, HERTZ_FORMAT);
  tCtl->attachInterrupt(isrCtl);
  tCtl->setInterruptPriority(2, 0);
  tCtl->resume();

  ser.println("# Péndulo invertido - Creality 4.2.2");
  ser.println("# Arranque con el péndulo colgando (abajo) y quieto");
  ayuda();
}

void loop() {
  leerSerie();
  if (tickCtl) {
    tickCtl = false;
    control();
  }
}
