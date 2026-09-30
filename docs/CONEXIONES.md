# Conexiones — Mecánicas y Eléctricas

## Idea clave

- **Motor a pasos (NEMA 17):** fijo en un **extremo del riel**; mueve el carro con la correa. Es el **actuador**.
- **Encoder:** montado **sobre el carro**; su eje **es el pivote del péndulo**. Solo **mide** el ángulo θ. No mueve nada.
- La placa lee θ (encoder) → calcula LQR → mueve el carro (stepper) para mantener el péndulo arriba.

---

## 1. Mecánica

### Vista frontal (todo el sistema)

```
                              péndulo (varilla)
                                   │
                                   │  θ
                                   │
                              ┌────●────┐  ● = eje encoder = pivote
                              │  CARRO  │
  [SW-IZQ]                    └─┬─────┬─┘                    [SW-DER]
 ══╪════════════════════════════╧═════╧═══════════════════════╪══ riel
   │                               ║ clamp                    │
 ┌─┴──┐  ═══════════════════════════╩══════════════════════ ┌─┴──┐
 │NEMA│◄──────────── correa GT2 (lazo cerrado) ───────────► │POLEA│
 │ 17 │  ══════════════════════════════════════════════════ │LOCA │
 └────┘  polea GT2 20T                                      └─────┘
 extremo izq.                                               extremo der.
```

### Vista lateral del carro (cómo va el encoder)

```
            varilla del péndulo
                   ║
                   ║   (sujeta al eje Ø8 con prisionero/abrazadera)
   ┌───────────────╫─────────────────┐
   │   ┌──────┐   ┌┴┐   ┌──────┐     │
   │   │608ZZ │═══│ │═══│608ZZ │═[acople 8→6]═[ENCODER]   ← encoder atornillado al carro
   │   └──────┘   └─┘   └──────┘     │         600 P/R
   │        eje Ø8 mm (pivote)       │
   │             CARRO               │
   └──┬───────────────────────────┬──┘
      │ patines / ruedas del riel │
   ═══╧═══════════════════════════╧═══ riel
            ▲ clamp de correa bajo el carro
```

Alternativa simple (péndulo ligero): varilla sujeta **directo al eje de 6 mm del encoder** con un cubo/abrazadera. Menos piezas, pero el rodamiento del encoder carga el peso; usar solo si la varilla pesa < 100 g.

### Reglas de montaje
1. El eje del péndulo debe ser **perpendicular al riel** (el péndulo oscila en el plano del movimiento).
2. Encoder y eje **alineados**; el acople flexible absorbe pequeños desalineamientos.
3. El péndulo debe girar **libre 360°** sin chocar con la correa ni el riel.
4. Correa tensa, sin juego: el juego entre correa y carro = error en x.
5. Switches a unos cm antes de los topes físicos.

---

## 2. Eléctrica

```
  220/110 VAC
      │
 [Interruptor + fusible]
      │
 ┌────┴─────┐        [PARO EMERGENCIA NC]
 │ FUENTE   │ +24V ───────────o o──────────┐
 │ 24V 5A   │ GND ─────────────────────┐   │
 └──────────┘                          │   │
                            ┌──────────┴───┴───────────────────────────┐
                            │   PLACA CREALITY 4.2.2   (entrada 24V)   │
                            │                                          │
  NEMA 17 ◄═══ 4 hilos ═════╡ X  (motor)                               │
                            │                                          │
  SW izq (NC) ── 2 hilos ───╡ X-STOP   señal + GND           (PA5)     │
                            │                                          │
  ENCODER  blanco A ────────╡ Y-STOP   señal                 (PA6)     │
           negro GND ───────╡ Y-STOP   GND                             │
           verde  B ────────╡ Z-STOP   señal                 (PA7)     │
           rojo  Vcc ───────╡ BLTouch  5V                              │
           malla ───────────╡ GND (solo en este extremo)               │
                            │                                          │
  SW der (NC) ── 2 hilos ───╡ BLTouch  IN + GND              (PB1)     │
                            │                                          │
  PC / MATLAB ◄── USB ──────╡ micro-USB                                │
                            └──────────────────────────────────────────┘
       No usar: HOTEND, CAMA, termistores, pantalla.
```

### Detalle de conectores

| Dispositivo | Cable | Conector placa | Pin |
|---|---|---|---|
| NEMA 17 | 4 hilos (2 bobinas) | X | — |
| Encoder | Rojo (Vcc) | BLTouch 5V | — |
| Encoder | Negro (GND) | Y-STOP GND | — |
| Encoder | Blanco (A) | Y-STOP señal | PA6 |
| Encoder | Verde (B) | Z-STOP señal | PA7 |
| Encoder | Malla | GND placa | — |
| Switch izq. | NC + COM | X-STOP señal + GND | PA5 |
| Switch der. | NC + COM | BLTouch IN + GND | PB1 |
| Paro emergencia | NC en serie | +24 V de fuente | — |

### Verificaciones antes de energizar
1. Encoder alimentado, **A y B desconectados**: medir A→GND girando el eje. No debe haber 5 V fijos (colector abierto). Si hay 5 V fijos → divisor 10k/20k.
2. Señal Y-STOP / Z-STOP sin nada conectado ≈ 3.3 V.
3. Motor: medir continuidad para identificar pares de bobina (≈ 1–3 Ω).
4. Polaridad de 24 V en la placa.
