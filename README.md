# Operation Belkanite — Reactive & Deliberative Multi-Agent AI (C++)

Autonomous multi-agent AI system developed in **C++** for a 2D/3D grid-based environmental containment simulator. Two heterogeneous agents — an **Engineer** and a **Technician** — explore unknown terrain, compute optimal paths, design gravity-fed pipeline networks under ecological constraints, and coordinate to build them.

> **Note on Architecture:** The base 3D/OpenGL simulation engine was provided by the Department of Computer Science and Artificial Intelligence (DECSAI, University of Granada). All AI logic, search algorithms, heuristics, and multi-agent coordination are implemented in `Comportamientos_Agentes/` (`ingeniero.cpp/.hpp` and `tecnico.cpp/.hpp`).

## Implemented Levels & Algorithms (Levels 0–5)

* **Levels 0 & 1 — Reactive Exploration (`mapaCalor`):** Heatmap-based reactive navigation prioritizing unvisited cells while targeting waste treatment plants (`'U'`) and gear upgrades (`'D'`). Achieved **100%** road/path discovery on `minivertiguito.map` and **99.65%** on `mapaop.map`.
* **Level 2 — Minimum-Step Pathfinding (BFS):** Breadth-First Search (`B_Anchura`) over state `(row, col, orientation, shoes)` to guarantee the minimum number of simulation steps for the Engineer, dynamically tracking elevation-boosted `WALK` and `JUMP` transitions.
* **Level 3 — Minimum-Energy Pathfinding (A\*):** A* search (`A_Estrella`) using a `std::priority_queue` minimizing terrain-dependent battery consumption (`CostoBateria`) guided by an admissible **Chebyshev distance** heuristic (`max(|Δrow|, |Δcol|)`).
* **Level 4 — Constrained Pipeline Planning (Dijkstra):** State-space Dijkstra search (`PlanificarRed`) over `(row, col, operation)` minimizing pipeline length (with ecological impact as tie-breaker) while enforcing gravity flow constraints ($\Delta h \le 1$), single-cell terrain modification limits (`DIG`/`RAISE`), and maximum ecological impact thresholds.
* **Level 5 — Multi-Agent Cooperative Construction (FSM):** Finite State Machine coordinating both agents. The Engineer acts as central planner and terrain modifier, summoning the Technician via `COME` signals. To prevent deadlocks in narrow corridors, the Technician uses a **temporary-wall collision avoidance** technique (marking the Engineer's radar position as an obstacle before running A*) before executing synchronized face-to-face `INSTALL` actions.

## Benchmark Highlights (Level 5)
| Map | Steps to Complete | Status |
| :--- | :---: | :---: |
| `mapa75_espirales.map` | 149 | Completed |
| `minivertiguito.map` | 336 | Completed |
| `luminalia25.map` | 679 | Completed |

## Build & Run (Linux / Ubuntu)

```bash
# Install dependencies and build (first time)
./install.sh

# Or compile with CMake & Make
cmake .
make -j

# Run with GUI
./practica2

# Run headless (batch mode example - Level 5)
./practica2SG -m ./mapas/mapa75_espirales.map -n 5 -i 69 69 7 -t 3 69 5 -seed 0 -Tiempo 3000 -Ambiental 1417 -Energia 4150 -O 16 10
