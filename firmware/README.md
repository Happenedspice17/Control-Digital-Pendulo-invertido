# Firmware — Creality 4.2.2 (STM32F103RET6)

Control LQR discreto (500 Hz) del péndulo invertido sobre carro con motor a pasos.
Entrada de control = **aceleración del carro**; el firmware la integra a velocidad → frecuencia de pasos.

```
matlab/parametros.m ──► matlab/diseno_lqr.m ──► firmware/include/gains.h ──► placa
```

## Cableado

| Función | Conector placa | Pin MCU | Nota |
|---|---|---|---|
| Motor carro | **X** | PC2 STEP / PB9 DIR / PC3 EN | Driver integrado, 1/16 µpaso |
| Encoder A | **Y-STOP** (señal) | PA6 — TIM3_CH1 | Contador en cuadratura por hardware |
| Encoder B | **Z-STOP** (señal) | PA7 — TIM3_CH2 | |
| Encoder GND | Y-STOP GND | — | |
| Encoder V+ (5 V) | Header BLTouch, pin 5 V | — | Encoder NPN colector abierto |
| Final de carrera izq. | **X-STOP** | PA5 | Switch NC a GND |
| Final de carrera der. | **BLTouch IN** | PB1 | Switch NC a GND (usar GND del header) |
| Hotend / cama / fan | — | PA1 / PA2 / PA0 | Forzados a LOW |
| USB | micro-USB | PA9/PA10 (CH340) | 115200 baud |

> **No conectar la salida del encoder a 5 V con pull-up propio:** PA6/PA7 no toleran 5 V. Usar encoder de **colector abierto (NPN)**; la placa ya tiene pull-up a 3.3 V.
>
> Verificar con multímetro antes de conectar: el pin de señal de Y-STOP/Z-STOP sin nada conectado debe marcar ≈ 3.3 V.

### Filtro RC de los conectores de final de carrera
Los conectores de endstop tienen un filtro RC. Si el péndulo pierde cuentas (ver *Prueba de encoder*), retirar el capacitor de las líneas PA6/PA7 o añadir pull-up externo de 1 kΩ a 3.3 V.

## Compilar y cargar

Requiere [PlatformIO](https://platformio.org/) (VS Code o CLI).

```bash
cd firmware
pio run
```

**Opción A — microSD (conserva bootloader de Creality):**
1. Copiar `.pio/build/creality422/firmware.bin` a la raíz de la SD con **nombre nuevo cada vez** (ej. `pend01.bin`, `pend02.bin`).
2. Insertar SD, encender, esperar ~10 s.

**Opción B — ST-Link V2 (header SWD):** `pio run -t upload`. Si se borra el bootloader, cambiar `board_build.offset` a `0x0`.

Monitor serie: `pio device monitor`.

## Comandos serie

| Cmd | Acción |
|---|---|
| `h` | Homing con el switch izquierdo → x = 0 al centro |
| `z` | x = 0 en la posición actual (sin homing) |
| `e` | Habilitar motor → estado LISTO |
| `d` | Deshabilitar motor (se pierde x = 0: repetir `h` o `z`) |
| `c` | Recalibrar encoder (péndulo colgando = abajo) |
| `w` | Onda cuadrada ±`REF_AMP` (como la demo de MathWorks) |
| `r0.05` | Referencia fija de posición en m |
| `t` | Telemetría CSV `t_ms,x,v,theta,omega,a,x_ref,estado` |
| `i` | Estado, ángulo, switches y ganancias |

## Máquina de estados

```
INACTIVO ──e──► LISTO ──|θ|<5° por 300 ms──► BALANCE
   ▲  ▲           ▲                            │
   │  └──h──HOMING┘◄──────|θ|>30° o |x|>X_SUAVE┘
   └──d── (cualquiera)     final de carrera ──► FALLA (motor off)
```

## Puesta en marcha

1. Encender con el péndulo **colgando y quieto** (define θ = π abajo).
2. `i` → girar el péndulo a mano: θ debe cambiar; inclinar hacia +x (lado contrario al switch izq.) debe dar θ > 0 cerca de arriba. Si no, cambiar `ENC_SIGNO`.
3. **Prueba de encoder:** dar 10 vueltas rápidas y dejarlo colgar; `i` debe mostrar |θ| ≈ 180° (±1°). Si deriva → ver filtro RC.
4. Ajustar Vref del driver X (~0.9–1.0 V para TMC2208 ≈ 1.1 A).
5. `h` (homing). Verificar que se mueva hacia el switch izquierdo; si no, cambiar `MOTOR_SIGNO`. Ajustar `X_SWITCH_IZQ` = distancia real centro→switch.
6. `e`, subir el péndulo a mano hasta vertical → entra en BALANCE.
7. `w` para seguir la onda cuadrada; `t` y `matlab/telemetria.m` para registrar.

## Sintonización

- Medir `m_var`, `L_var`, `m_tip`, `L_tip` → `matlab/parametros.m` → `diseno_lqr.m` (regenera `gains.h`).
- Oscila rápido / vibra → subir `N_VEL` o `TAU_W`, o bajar `Q(4,4)`.
- Se va lejos del centro → subir `Q(1,1)`.
- Pierde pasos → bajar `A_MAX`, subir Vref o usar 24 V.
