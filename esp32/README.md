# ESP32 — Dashboard WiFi

El ESP32 (WROOM-32 DevKit) crea su propia red WiFi y sirve un dashboard web. Hace de **puente** entre el celular y la Creality: el control sigue corriendo en la Creality y el WiFi nunca entra al lazo.

```
Celular (navegador) ◄─ WiFi / WebSocket ─► ESP32 ◄─ UART 460800 ─► Creality 4.2.2 (LQR 500 Hz)
```

## Uso

1. Encender todo.
2. En el celular, conectarse a la red **`Pendulo-Invertido`** (contraseña **`pendulo123`**).
3. Abrir **http://192.168.4.1**
4. Si el celular avisa "sin internet", elegir **mantener conexión** (y, si hace falta, apagar datos móviles).

No requiere internet ni la red de la escuela; la página está embebida en el ESP32 (sin librerías externas).

## Dashboard

- Estado, θ, x, a en grande
- Gráficas en vivo (10 s): θ, x vs x_ref, a — ~100 muestras/s
- Botones: **DESHABILITAR**, Homing, x = 0, Habilitar, Calibrar θ, Onda cuadrada, Info
- Slider de referencia (±150 mm)
- Ganancias K editables en vivo
- Grabar y descargar CSV (para MATLAB)
- Log de mensajes de la placa

> El paro de emergencia **físico** sigue siendo obligatorio. El WiFi puede cortarse.

## Latencia

| Tramo | Tiempo |
|---|---|
| Creality → ESP32 (460800 baud) | ~1.5 ms |
| Agrupación de muestras | 0–40 ms |
| WiFi (WebSocket, sin ahorro de energía) | 5–30 ms |
| Dibujo en el celular | ~16 ms |
| **Total** | **~30–90 ms** |

## Compilar y cargar

```bash
cd esp32
pio run -t upload
pio device monitor      # muestra la IP y los mensajes de la placa
```

`web/index.html` es la página; `web/generar.py` la convierte a `include/pagina.h` en cada compilación.
Para editar la interfaz sin el ESP32: abrir `web/index.html` en el navegador (mostrará "desconectado").

## Configuración (`src/main.cpp`)

| Constante | Valor | |
|---|---|---|
| `AP_SSID` / `AP_PASS` | Pendulo-Invertido / pendulo123 | Cambiar si hay varios equipos |
| `AP_CANAL` | 6 | Cambiar si hay interferencia |
| `PIN_RX2` / `PIN_TX2` | GPIO16 / GPIO17 | UART2 |
| `BAUD_LINK` | 460800 | Igual a `BAUD_ESP` en `firmware/include/config.h` |

## Protocolo

- Creality → ESP32: líneas CSV `t_ms,x,v,theta,omega,a,x_ref,estado` y mensajes que empiezan con `#`.
- ESP32 → celular: `T` + lote de líneas CSV cada 40 ms; `M` + mensaje.
- Celular → ESP32 → Creality: los mismos comandos del puerto USB (`h`, `e`, `d`, `r0.05`, `k-13.8,-11,-49.7,-8`, ...).
- Si el ESP32 no recibe datos por 1 s, envía `t1` para activar la telemetría.
