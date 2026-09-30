#pragma once

// ---- Pines Creality 4.2.2 (Marlin pins_CREALITY_V4.h) ----
#define PIN_STEP      PC2        // X_STEP
#define PIN_DIR       PB9        // X_DIR
#define PIN_EN        PC3        // enable común, activo en LOW
#define PN_STEP       PC_2
#define PN_DIR        PB_9

#define PIN_ENC_A     PA6        // Y_STOP  -> TIM3_CH1
#define PIN_ENC_B     PA7        // Z_STOP  -> TIM3_CH2
#define PIN_LIM_IZQ   PA5        // X_STOP
#define PIN_LIM_DER   PB1        // BLTouch IN
#define LIM_ACTIVO    HIGH       // switch NC a GND: presionado/cable roto = HIGH

#define PIN_HOTEND    PA1        // se fuerzan a LOW
#define PIN_CAMA      PA2
#define PIN_FAN       PA0

#define PIN_RX        PA10       // USART1 -> CH340 (USB)
#define PIN_TX        PA9
#define BAUD          115200

// ---- Mecánica ----
#define PASOS_REV     200
#define MICROPASOS    16         // fijo en 4.2.2 (standalone)
#define DIENTES       20
#define PASO_CORREA   0.002f
#define PASOS_M       (PASOS_REV * MICROPASOS / (DIENTES * PASO_CORREA))
#define MOTOR_SIGNO   1          // invertir si +x va al lado contrario

// ---- Encoder ----
#define ENC_PPR       1000
#define ENC_CPR       (4 * ENC_PPR)
#define ENC_SIGNO     1          // invertir si theta crece al inclinar hacia -x

// ---- Control ----
#define STEP_TICK_HZ  80000      // máx. 40 kpasos/s = 0.5 m/s
#define A_MAX         10.0f      // m/s^2
#define V_MAX         0.45f      // m/s
#define A_FRENO       3.0f       // m/s^2 al detenerse fuera de BALANCE
#define X_SUAVE       0.20f      // m, límite por software desde el centro
#define ANG_ARMAR     0.087f     // rad (5°) entra a BALANCE
#define T_ARMAR_MS    300
#define ANG_CAIDA     0.52f      // rad (30°) sale de BALANCE
#define N_VEL         5          // ventana derivada theta
#define TAU_W         0.004f     // s filtro omega

// ---- Homing ----
#define USAR_HOMING   1
#define V_HOMING      0.05f      // m/s
#define X_SWITCH_IZQ  -0.25f     // m, posición del switch izq. respecto al centro

// ---- Referencia tipo demo MathWorks ----
#define REF_AMP       0.10f      // m
#define REF_T         8.0f       // s

#define TELEM_DIV     10         // telemetría a CTL_HZ/TELEM_DIV
