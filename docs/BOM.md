# BOM — Péndulo Invertido sobre Carro (Arduino)

Referencia: [MathWorks — Inverted Pendulum with Animation](https://es.mathworks.com/help/simulink/slref/inverted-pendulum-with-animation.html)

**Ya disponible:** riel lineal, correa Gates (se asume perfil **GT2, 6 mm**; ajustar poleas si es otro perfil).

---

## 1. Selección de actuador

| Criterio | Servo RC (SG90/MG996R) | Stepper NEMA 17 + TMC2209 | Stepper NEMA 17 lazo cerrado (MKS SERVO42C/D) | Motor DC + encoder (JGB37 / 775) |
|---|---|---|---|---|
| Rotación continua | ❌ (180°) | ✅ | ✅ | ✅ |
| Respuesta rápida | ❌ | ✅ | ✅ | ✅✅ |
| Pérdida de pasos | — | ⚠️ posible | ✅ corregida | — |
| Control por fuerza/torque (modelo MathWorks) | ❌ | ❌ (control por aceleración) | ⚠️ | ✅ (PWM ≈ fuerza) |
| Facilidad | — | ✅✅ | ✅ | ✅ |
| **Veredicto** | **No apto** | **Recomendado (opción A)** | Mejor stepper (opción A+) | Alternativa (opción B) |

> Los servos RC **no sirven** para mover el carro: rango limitado y control de posición lento. Si se quiere "servo", usar un **servo industrial / stepper de lazo cerrado** (opción A+).

---

## 2. BOM — Opción A: Stepper (recomendada)

### 2.1 Control y electrónica

| # | Componente | Especificación | Cant. | Precio aprox. (USD) | Nota |
|---|---|---|---|---|---|
| 1 | Arduino Mega 2560 | ATmega2560, 16 MHz, 6 INT externas | 1 | 15 | Alternativa: Arduino Due / Teensy 4.0 (más velocidad) |
| 2 | Motor paso a paso NEMA 17 | 1.8°, ≥ 0.45 N·m (17HS19-2004S1, 2 A) | 1 | 14 | Eje 5 mm |
| 3 | Driver TMC2209 | StepStick, UART, hasta 2 A RMS | 1 (+1 repuesto) | 6 | Silencioso; alt. DM542 (NEMA 23) |
| 4 | Encoder incremental péndulo | LPD3806-600BM, 600 PPR, A/B, 5–24 V | 1 | 15 | 2400 cuentas/rev en cuadratura (0.15°) |
| 5 | Fuente conmutada | 24 V, 5 A (120 W) | 1 | 18 | 12 V funciona pero con menor velocidad |
| 6 | Convertidor buck | LM2596 24 V → 5 V/7 V | 1 | 2 | Alimentar Arduino/encoder |
| 7 | Capacitor electrolítico | 100 µF, 50 V | 2 | 1 | En VMOT del driver |
| 8 | Finales de carrera | Microswitch con palanca (KW12-3) | 2 | 2 | Seguridad + homing |
| 9 | Botón paro de emergencia | Hongo NC, 22 mm | 1 | 4 | Corta VMOT |
| 10 | Pulsadores | 12 mm (Start / Reset) | 2 | 1 | |
| 11 | Shield / PCB perforada | Perfboard 7×9 cm o CNC Shield V3 | 1 | 4 | |
| 12 | Resistencias pull-up | 4.7 kΩ (encoder A/B si open-collector) | 4 | 0.5 | LPD3806 es colector abierto |
| 13 | Disipador + ventilador | 40 mm 24 V para driver | 1 | 3 | |
| 14 | Borneras y conectores | KF301, JST-XH 4P, Dupont | kit | 5 | |
| 15 | Cable | AWG 18 (potencia), AWG 24 (señal), apantallado 4 hilos (encoder) | 3 m c/u | 6 | |
| 16 | Interruptor principal | Basculante con fusible, IEC C14 | 1 | 3 | |

### 2.2 Mecánica

| # | Componente | Especificación | Cant. | Precio aprox. (USD) | Nota |
|---|---|---|---|---|---|
| 17 | Polea motriz GT2 | 20 dientes, 6 mm, bore 5 mm | 1 | 2 | r ≈ 6.37 mm → 40 mm/rev |
| 18 | Polea loca GT2 | 20T sin dientes, bore 5 mm, con rodamiento | 1 | 2 | Extremo opuesto |
| 19 | Tensor de correa | Tensor GT2 de resorte o soporte ranurado | 1 | 2 | |
| 20 | Soporte motor NEMA 17 | Escuadra de aluminio / impresa | 1 | 3 | |
| 21 | Carro | Placa aluminio 3–5 mm o impresa PETG, compatible con riel | 1 | 5 | Minimizar masa (M) |
| 22 | Sujetador de correa | Clamp GT2 para carro | 2 | 1 | |
| 23 | Eje del péndulo | Acero plata Ø 8 mm, 60 mm | 1 | 2 | Acople directo al encoder |
| 24 | Rodamientos | 608ZZ (8×22×7) | 2 | 1 | Soporte del eje en el carro |
| 25 | Acople flexible | 6 mm ↔ 8 mm (eje encoder ↔ eje péndulo) | 1 | 2 | Evita carga radial en encoder |
| 26 | Varilla del péndulo | Aluminio Ø 6–8 mm o tubo, 300–500 mm | 1 | 3 | Longitud L del modelo |
| 27 | Masa en la punta (opcional) | Tuerca / disco 20–50 g | 1 | 1 | Para ajustar m y l |
| 28 | Topes mecánicos | Bumpers de goma en extremos del riel | 2 | 1 | |
| 29 | Tornillería | M3, M4, M5 + tuercas T (si riel V-slot) | kit | 5 | |
| 30 | Base | MDF 18 mm / perfil 2040, largo = riel | 1 | 8 | |

**Subtotal estimado opción A:** ≈ 175 USD

### 2.3 Opción A+ (stepper de lazo cerrado)

Sustituir ítems **2, 3, 13**:

| Componente | Especificación | Cant. | Precio aprox. (USD) |
|---|---|---|---|
| MKS SERVO42D (o 57D con NEMA 23) | NEMA 17 + encoder magnético + driver integrado, STEP/DIR | 1 | 35 |

---

## 3. BOM — Opción B: Motor DC con encoder (control por fuerza)

Sustituir ítems **2, 3, 13, 17** por:

| Componente | Especificación | Cant. | Precio aprox. (USD) | Nota |
|---|---|---|---|---|
| Motor DC con encoder | JGB37-520 12 V 330 rpm, encoder Hall 11 PPR | 1 | 15 | O 775 + encoder 600 PPR externo |
| Driver puente H | BTS7960 43 A | 1 | 8 | Alt. VNH5019 / L298N (no recomendado) |
| Polea GT2 20T | bore 6 mm (eje del JGB37) | 1 | 2 | |
| Fuente | 12 V, 10 A | 1 | 18 | Sustituye ítem 5 |

> Con motor DC el Arduino Mega necesita 4 interrupciones (encoder péndulo A/B + encoder motor A/B): pines 2, 3, 18, 19.

---

## 4. Herramientas / consumibles

- Impresora 3D (PETG) o taller para placa del carro y soportes
- Multímetro, cautín, estaño, termofil
- Llaves Allen, calibrador
- Loctite 243 (tornillos prisioneros de polea)

---

## 5. Asignación de pines (Arduino Mega, opción A)

| Señal | Pin |
|---|---|
| Encoder péndulo A | D2 (INT) |
| Encoder péndulo B | D3 (INT) |
| STEP | D9 (Timer1 / OC1A) |
| DIR | D8 |
| EN | D7 |
| TMC2209 UART | D18 (TX1) / D19 (RX1) vía 1 kΩ |
| Final carrera izq. | D20 |
| Final carrera der. | D21 |
| Start / Reset | D22 / D23 |

---

## 6. Parámetros a medir (para el modelo de Simulink)

| Símbolo | Descripción | Cómo |
|---|---|---|
| M | Masa del carro (+ partes móviles) | Báscula |
| m | Masa del péndulo | Báscula |
| l | Distancia pivote–centro de masa | Balance en filo |
| I | Inercia del péndulo | ≈ m·L²/3 (varilla) o prueba de oscilación |
| b | Fricción del carro | Prueba de desaceleración libre |
| r | Radio de la polea | 20T GT2 → 6.37 mm |

---

## 7. Notas de diseño

- **Frecuencia de control:** ≥ 200 Hz (Ts ≤ 5 ms). Usar interrupción por timer para el lazo.
- **Stepper:** el controlador (LQR/PID) debe generar **aceleración** del carro; integrar para obtener frecuencia de pasos. Microstepping 1/8 → 1600 pasos/rev → 25 µm/paso.
- **Velocidad máx. estimada:** NEMA 17 a 24 V ≈ 600 rpm → 0.4 m/s con polea 20T.
- **Swing-up** (como en el ejemplo MathWorks) requiere riel ≥ 0.8 m y buena aceleración; con solo estabilización basta ≥ 0.5 m.
- Verificar perfil real de la correa Gates (GT2 2 mm / GT3 / HTD 3M) y ancho antes de comprar poleas.
