# Falling Sand — Cellular Automaton Sand Simulation

An interactive falling-sand toy written in C++ with [raylib](https://www.raylib.com/), running on a Raspberry Pi 5 with a DSI touchscreen. Touch to pour sand and watch it fall, spread, and pile.

Where the [double pendulum](../double-pendulum) in this collection simulates *continuous* physics by integrating differential equations, this project sits at the opposite end of computational physics: a **cellular automaton**, where complex behavior emerges from repeatedly applying one trivially simple local rule to a grid of cells. No equations of motion, no integrator — just a rule and a grid.

---

## The model

The world is a fixed 2D grid of cells. Each cell is either empty or contains sand. Every frame, each sand cell is evaluated against a short rule hierarchy:

1. **Fall straight down** if the cell below is empty. Gravity always wins.
2. **Slide diagonally** if straight down is blocked — try down-left or down-right.
3. **Stay put** if nothing is available. The grain is stuck.

That's the entire "physics." The piling, the slopes, the way sand flows around obstacles and settles — none of it is programmed. It's **emergent**, arising from thousands of cells each following those three lines of logic.

---

## The two things that actually matter

The rule is easy. Getting it *right* comes down to two non-obvious details.

### 1. Update order determines correctness

The grid must be scanned **bottom-to-top**.

Scanning top-to-bottom seems natural, but it breaks the simulation: move a grain from row 5 to row 6, and the loop then *arrives* at row 6, finds that same grain, and moves it again — and again — teleporting it to the floor in a single frame. Sand would drop instantly instead of falling.

Scanning bottom-to-top means a moved grain lands in a row the loop has **already passed**, so each grain moves exactly one cell per frame. That's what produces visible falling. Same rule, reversed sweep, completely different behavior.

### 2. Randomizing the slide direction

If the diagonal check always tries left before right, then every time both diagonals are open, sand goes left — every grain, every frame. The result is a pile that visibly leans, with a vertical wall on one side and a slope on the other: a right triangle instead of a cone.

The bias comes from the *code's* scan order, not from any physics. Fixing it means flipping a coin to decide which diagonal to attempt **first**, with a fallback to the other if the first is blocked. The coin picks the preference order; the sand still only moves where there's actually room. Piles become symmetric.

---

## Grid mechanics

- **Fixed grid, mutable state.** The grid is allocated once at full size; cells are never added or removed, only toggled between empty and full. Cell *positions are implied by their indices* (`grid[row][col]` draws at `col * CELL, row * CELL`) rather than stored — so when a grain moves between cells, its position follows for free with nothing to keep in sync.
- **Bounds checking on the destination.** Every grid access derived from a moving quantity — a falling grain, a diagonal slide, a brush stroke near an edge — must be checked against the grid's limits *before* the access. The rule of thumb: bounds-check the cell you're about to **touch**, not the one you're standing on.
- **Circular brush.** Painting sweeps a square neighborhood of offsets and keeps only those within a radius (comparing squared distances to avoid a needless square root), giving a round brush from a square loop.
- **State lives outside the loop.** Anything the user changes and expects to persist (brush size) is declared outside the frame loop — declare it inside and it silently resets every frame.

---

## Controls

- **Touch / click and drag** anywhere on the canvas to pour sand.
- **+ / −** buttons adjust the brush radius (1–20 cells).
- **Reset** clears the grid.

---

## Building

```bash
mkdir build && cd build
cmake ..
make
./fallingSand
```

Requires raylib (on the Raspberry Pi, built from source with the OpenGL ES 2.0 backend). Run from a terminal with access to a display.

---

## Possible extensions

The falling-sand genre is a deep rabbit hole. Natural next steps:

- **More materials.** Water (spreads horizontally, seeks its own level), stone (immovable), oil (floats on water). Each is just a different rule set on the same grid — the cell would hold a *material type* instead of a bool.
- **Angle of repose.** Make diagonal sliding probabilistic: a grain that sometimes declines to slide forms steeper piles. This is a crude but real model of the maximum slope a granular material can hold (~34° for dry sand), tunable with a single probability.
- **Performance.** At small cell sizes the grid gets large; only scanning regions that contain active particles avoids sweeping thousands of empty cells every frame.