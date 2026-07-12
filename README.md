# Physics Simulations

A collection of real-time, interactive physics simulations written in C++ with [raylib](https://www.raylib.com/), built and run on a Raspberry Pi 5 with a DSI touchscreen. Each project focuses on a different corner of computational physics — from continuous differential-equation integration to discrete cellular automata — with an emphasis on getting the *physics* right, not just the visuals.

Each sub-project is self-contained and independently buildable (its own `CMakeLists.txt`).

---

## Projects

### [Double Pendulum](./double-pendulum)
A chaotic double pendulum integrating the true coupled equations of motion. Built in three iterations — an uncoupled approximation, Euler–Cromer integration, and RK4 — with a live energy-drift readout used to measure and compare integrator accuracy (RK4 brings energy drift from 200%+ down to near zero). Interactive: drag any joint to set initial conditions on the touchscreen.

*Topics: coupled ODEs, RK4 vs. Euler–Cromer, energy conservation, chaos, camera-space rendering.*

### [Falling Sand](./falling-sand)
A cellular-automaton sand simulation — a fixed grid where each cell follows one trivial local rule (fall down; if blocked, slide diagonally; otherwise stay), producing emergent piling and flow behavior with no equations of motion at all. The discrete counterpart to the double pendulum's continuous physics. Touch to pour sand, with an adjustable brush size.

The interesting problems turned out to be bookkeeping rather than physics: the grid must be scanned bottom-to-top (top-to-bottom makes every grain teleport to the floor in a single frame), and the diagonal slide direction must be randomized with a fallback, or piles develop a directional bias and come out as right triangles instead of cones.

*Topics: cellular automata, grid-based simulation, update-order correctness, emergent behavior, bounds checking.*

---

## Building

Each project builds independently with CMake. From a project's folder:

```bash
mkdir build && cd build
cmake ..
make
```

raylib must be installed (on the Raspberry Pi, built from source with the OpenGL ES 2.0 backend). Run each program from a terminal with access to a display.

---

## Environment

- Raspberry Pi 5 (8 GB)
- Freenove 5" DSI touchscreen
- Raspberry Pi OS (Bookworm, 64-bit)
- raylib compiled from source (`GRAPHICS_API_OPENGL_ES2`)