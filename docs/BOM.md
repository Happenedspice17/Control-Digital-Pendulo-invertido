# BOM final — Péndulo invertido sobre carro

Configuración elegida: **Creality 4.2.2 + NEMA 17 + encoder 600 PPR + ESP32 (dashboard WiFi)**.
Precios aproximados en MXN (solo lo que hay que comprar). Comparativa de alternativas: [`BOM_opciones.md`](BOM_opciones.md).

## 1. Electrónica y control

| # | Componente | Especificación | Cant. | Estado | Precio aprox. |
|---|---|---|---|---|---|
| 1 | Placa Creality 4.2.2 | STM32F103RET6, drivers integrados (1/16) | 1 | ✅ Disponible | — |
| 2 | Motor a pasos NEMA 17 | 1.8°, ≥ 0.4 N·m | 1 | ✅ Disponible | — |
| 3 | Fuente 24 V | ≥ 5 A (de la Chiron) | 1 | ✅ Disponible | — |
| 4 | Finales de carrera | Microswitch, cableado NC a GND | 2 | ✅ Disponible (Chiron) | — |
| 5 | **Encoder incremental** | **E38S6G5-600B-G24N**, 600 PPR, NPN colector abierto, 5–24 V, eje 6 mm | 1 | 🛒 Comprar | $246 |
| 6 | ESP32 DevKit V1 | WROOM-32, 38 pines | 1 | ✅ Disponible | — |
| 7 | Convertidor buck | LM2596, 24 V → 5 V (alimenta ESP32) | 1 | 🛒 Comprar | $50 |
| 8 | Tarjeta microSD | ≤ 32 GB, FAT32 (flashear la Creality) | 1 | 🛒 Si no hay | $100 |
| 9 | ST-Link V2 *(opcional)* | Programar/depurar por SWD | 1 | 🛒 Opcional | $150 |

## 2. Mecánica

| # | Componente | Especificación | Cant. | Estado | Precio aprox. |
|---|---|---|---|---|---|
| 10 | Riel | Perfil de aluminio (Chiron) | 1 | ✅ Disponible | — |
| 11 | Correa Gates | GT2 6 mm, lazo cerrado | 1 | ✅ Disponible | — |
| 12 | Polea motriz GT2 | 20 dientes, bore 5 mm | 1 | ✅ Disponible (Chiron) | — |
| 13 | Polea loca | GT2 o lisa, con 608ZZ | 1 | ✅ Disponible (Chiron / 608ZZ) | — |
| 14 | Rodamientos 608ZZ | 8×22×7 mm (pivote del péndulo) | 2 | ✅ Disponible | — |
| 15 | Eje del pivote | Acero plata Ø 8 mm × 60 mm | 1 | 🛒 Comprar | $60 |
| 16 | Acople flexible | 6 mm ↔ 8 mm (encoder ↔ eje) | 1 | 🛒 Comprar | $60 |
| 17 | Varilla del péndulo | Aluminio Ø 6–8 mm × 400 mm | 1 | 🛒 Comprar | $100 |
| 18 | Piezas impresas | Carro, soporte encoder, cubo del péndulo, soporte motor, tensor, clamps de correa (PETG) | 1 juego | ✅ Imprimir (A2L / Ender) | — |
| 19 | Tornillería | M2–M5, tuercas, resortes | kit | ✅ Disponible | — |
| 20 | Topes de goma | Extremos del riel | 2 | 🛒 Comprar | $30 |

## 3. Cableado y seguridad

| # | Componente | Especificación | Cant. | Estado | Precio aprox. |
|---|---|---|---|---|---|
| 21 | Paro de emergencia | Hongo NC, 22 mm, corta +24 V | 1 | 🛒 Comprar | $80 |
| 22 | Interruptor de entrada | Con fusible (IEC C14 o basculante) | 1 | 🛒 Si la fuente no trae | $50 |
| 23 | Conectores JST-XH 2.54 | 2 y 3 pines (endstops, encoder a la placa) | kit | 🛒 Comprar | $60 |
| 24 | Cables Dupont | H-H / H-M (ESP32 ↔ placa) | 10 | 🛒 Comprar | $30 |
| 25 | Cable | AWG 18 (24 V), AWG 24 (señales) | 2 m c/u | ✅ / 🛒 | $40 |
| 26 | Termofil y cinchos | Varios | — | ✅ / 🛒 | $30 |

## 4. Resumen de compra

| Concepto | Precio aprox. |
|---|---|
| Indispensable (5, 7, 15, 16, 17, 20, 21, 23, 24) | **≈ $720** |
| Si no hay microSD / interruptor / cable / termofil (8, 22, 25, 26) | + $220 |
| ST-Link (opcional) | + $150 |
| **Total máximo** | **≈ $1,090 MXN** |

## 5. Herramientas

Multímetro, cautín y estaño, desarmadores, llaves Allen, calibrador, impresora 3D, Loctite 243 (prisioneros de poleas y acople).

## 6. Notas

- **Encoder:** verificar al recibirlo que la salida es colector abierto (A sin 5 V fijos con el encoder alimentado). Conexión en [`CONEXIONES.md`](CONEXIONES.md).
- **Pivote:** eje Ø8 en 2×608ZZ en el carro, varilla sujeta al eje, encoder acoplado con el acople flexible (el encoder no carga peso).
- **Masa del carro:** mientras más ligero, más aceleración disponible del NEMA 17.
- Medir el péndulo final (masa y largo) y actualizar `matlab/parametros.m`.
