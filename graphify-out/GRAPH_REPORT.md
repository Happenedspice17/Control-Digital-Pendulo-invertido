# Graph Report - Control-Digital-Pendulo-invertido  (2026-11-01)

## Corpus Check
- 17 files · ~8,058 words
- Verdict: corpus is large enough that graph structure adds value.
- Unclassified: 4 file(s) not represented in the graph (top: (none) 2, .ini 2)

## Summary
- 110 nodes · 145 edges · 14 communities (8 shown, 6 thin omitted)
- Extraction: 100% EXTRACTED · 0% INFERRED · 0% AMBIGUOUS
- Token cost: 0 input · 0 output

## Graph Freshness
- Built from commit: `fbac0984`
- Run `git rev-parse HEAD` and compare to check if the graph is stale.
- Run `graphify update .` after code changes (no API cost).

## Community Hubs (Navigation)
- firmware/src/main.cpp
- BOM — Opciones evaluadas (referencia)
- esp32/src/main.cpp
- Conexiones — Mecánicas y Eléctricas
- Firmware — Creality 4.2.2 (STM32F103RET6)
- BOM_opciones.md
- BOM final — Péndulo invertido sobre carro
- ESP32 — Dashboard WiFi
- Estado
- Puerto
- Ambos
- CLAUDE.md
- .claude/CLAUDE.md

## God Nodes (most connected - your core abstractions)
1. `comando()` - 13 edges
2. `BOM — Opciones evaluadas (referencia)` - 11 edges
3. `control()` - 10 edges
4. `Estado` - 7 edges
5. `Puerto` - 7 edges
6. `BOM final — Péndulo invertido sobre carro` - 7 edges
7. `ESP32 — Dashboard WiFi` - 7 edges
8. `Firmware — Creality 4.2.2 (STM32F103RET6)` - 7 edges
9. `cambiar()` - 5 edges
10. `info()` - 5 edges

## Surprising Connections (you probably didn't know these)
- `cambiar()` --references--> `Estado`  [EXTRACTED]
  firmware/src/main.cpp → firmware/src/main.cpp  _Bridges community 8 → community 0_
- `comando()` --references--> `Puerto`  [EXTRACTED]
  firmware/src/main.cpp → firmware/src/main.cpp  _Bridges community 9 → community 0_
- `control()` --calls--> `telemetria()`  [EXTRACTED]
  firmware/src/main.cpp → firmware/src/main.cpp  _Bridges community 10 → community 0_

## Import Cycles
- None detected.

## Communities (14 total, 6 thin omitted)

### Community 0 - "firmware/src/main.cpp"
Cohesion: 0.18
Nodes (19): ayuda(), cambiar(), comando(), control(), encCero(), encInit(), encLeer(), envolver() (+11 more)

### Community 1 - "BOM — Opciones evaluadas (referencia)"
Cohesion: 0.14
Nodes (14): 10. Dashboard WiFi (ESP32), 1. Selección de actuador, 2.1 Control y electrónica, 2.2 Mecánica, 2.3 Opción A+ (stepper de lazo cerrado), 2. BOM — Opción A: Stepper (recomendada), 3. BOM — Opción B: Motor DC con encoder (control por fuerza), 4. Herramientas / consumibles (+6 more)

### Community 3 - "Conexiones — Mecánicas y Eléctricas"
Cohesion: 0.20
Nodes (10): 1. Mecánica, 2. Eléctrica, 3. ESP32 (dashboard WiFi), Conexiones — Mecánicas y Eléctricas, Detalle de conectores, Idea clave, Reglas de montaje, Verificaciones antes de energizar (+2 more)

### Community 4 - "Firmware — Creality 4.2.2 (STM32F103RET6)"
Cohesion: 0.25
Nodes (8): Cableado, Comandos serie, Compilar y cargar, Filtro RC de los conectores de final de carrera, Firmware — Creality 4.2.2 (STM32F103RET6), Máquina de estados, Puesta en marcha, Sintonización

### Community 6 - "BOM final — Péndulo invertido sobre carro"
Cohesion: 0.29
Nodes (7): 1. Electrónica y control, 2. Mecánica, 3. Cableado y seguridad, 4. Resumen de compra, 5. Herramientas, 6. Notas, BOM final — Péndulo invertido sobre carro

### Community 7 - "ESP32 — Dashboard WiFi"
Cohesion: 0.29
Nodes (7): Compilar y cargar, Configuración (`src/main.cpp`), Dashboard, ESP32 — Dashboard WiFi, Latencia, Protocolo, Uso

### Community 8 - "Estado"
Cohesion: 0.33
Nodes (6): Estado, BALANCE, FALLA, HOMING, INACTIVO, LISTO

### Community 9 - "Puerto"
Cohesion: 0.33
Nodes (5): Puerto, buf, div, n, telem

## Knowledge Gaps
- **9 isolated node(s):** `INACTIVO`, `HOMING`, `LISTO`, `BALANCE`, `FALLA` (+4 more)
  These have ≤1 connection - possible missing edges or undocumented components. (Counts symbols only; 65 node(s) total have ≤1 connection when file, concept and rationale nodes are included.)
- **6 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **Why does `BOM — Opciones evaluadas (referencia)` connect `BOM — Opciones evaluadas (referencia)` to `BOM_opciones.md`?**
  _High betweenness centrality (0.098) - this node is a cross-community bridge._
- **What connects `INACTIVO`, `HOMING`, `LISTO` to the rest of the system?**
  _9 weakly-connected nodes found - possible documentation gaps or missing edges._
- **Should `BOM — Opciones evaluadas (referencia)` be split into smaller, more focused modules?**
  _Cohesion score 0.14285714285714285 - nodes in this community are weakly interconnected._
- **Why does `Conexiones — Mecánicas y Eléctricas` connect `Conexiones — Mecánicas y Eléctricas` to `BOM_opciones.md`?**
  _High betweenness centrality (0.070) - this node is a cross-community bridge._
- **Why does `Firmware — Creality 4.2.2 (STM32F103RET6)` connect `Firmware — Creality 4.2.2 (STM32F103RET6)` to `BOM_opciones.md`?**
  _High betweenness centrality (0.057) - this node is a cross-community bridge._