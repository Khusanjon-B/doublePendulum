# Lorenz Attractor — Interactive 3D Chaos Explorer

A real-time, interactive visualization of the Lorenz system, written in C++ with [raylib](https://www.raylib.com/) and running on a Raspberry Pi 5 with a touchscreen. The attractor is integrated live with RK4 and rendered in 3D; on-screen sliders let you rotate the view and — more interestingly — change the system's parameters and watch its qualitative behavior transform in real time.

This is the third project in a series of physics simulations built on the Pi. Where the [double pendulum](../double-pendulum) integrates coupled ODEs from mechanics and the [falling sand](../falling-sand) is a discrete cellular automaton, this one visualizes a system from **dynamical systems / chaos theory** — and leans more heavily on building a real interactive UI than the earlier two.

---

## The Lorenz system

The Lorenz equations are three coupled first-order ODEs, originally derived by Edward Lorenz in 1963 as a drastically simplified model of atmospheric convection:

```
dx/dt = σ (y − x)
dy/dt = x (ρ − z) − y
dz/dt = x y − β z
```

With the classic parameters (σ = 10, ρ = 28, β = 8/3) the trajectory traces the famous butterfly-shaped **strange attractor**: a path that never settles, never repeats, and never escapes — confined forever to an intricate region of space while remaining chaotically unpredictable within it. It is one of the founding examples of deterministic chaos, and the origin of the term "butterfly effect."

## Why the parameters make it interesting

The reason this is built as an *interactive* explorer rather than a static render is that the Lorenz system exhibits qualitatively different behavior depending on its parameters — the kind of transient and long-term behavior (fixed points, stable and unstable equilibria, saddle behavior, and full chaos) that shows up throughout differential-equations and physics coursework.

The **ρ slider** is the most striking: below roughly ρ ≈ 24.74 the system is *not* chaotic — trajectories spiral into a stable fixed point. Cross that threshold and the fixed points lose stability and the chaotic attractor blooms into existence. Dragging ρ across that value and watching the butterfly appear and collapse is a live demonstration of a bifurcation — a concept that's abstract on paper and immediate on screen.

Slider ranges are chosen to keep the interesting behavior visible without wandering into blown-up numerical mess:

- **ρ (rho):** 0–50, default 28 — spans the transition from decay, through steady convection, into chaos.
- **σ (sigma):** 0–20, default 10 — reshapes the attractor.
- **β (beta):** 0–5, default 8/3 — adjusts the attractor's proportions.

---

## Implementation notes

### Integration
The system is stepped forward with **RK4** — the same integrator used for the double pendulum in this collection, reused here almost verbatim. The Lorenz state is simpler than the pendulum's (three scalars, no position/velocity split), so the `derivatives(State) → State` function is a near-direct transcription of the three equations. A fixed timestep (rather than frame time) is used, since the goal is an evenly-sampled curve rather than real-time dynamics.

### Coordinate conventions (the fiddly part)
Two different "up" conventions had to be reconciled:
- The Lorenz system is conventionally drawn with **z** as the vertical axis (the two wings separate along z, which oscillates around ρ−1 ≈ 27).
- raylib's 3D world uses **y** as up, with the ground grid in the x–z plane.

So each simulated point is transformed on the way to the screen: the sim's z maps to raylib's y (standing the butterfly upright), a constant offset of ~27 recenters it on the origin, and a scale factor fits it to the view. This "map simulation units into world units" step — scale, offset, axis-swap — is the same class of transform used for the grid-to-pixel mapping in the sand sim and the world-to-camera mapping in the pendulum.

### 3D rendering and camera
Uses raylib's `Camera3D` and `DrawLine3D`, drawing the trajectory as a connected trail of 3D line segments. The camera **orbits** the (stationary) attractor using spherical coordinates driven by two sliders — one for azimuth (sweeping around), one for elevation (tilting up and down). The horizontal components are scaled by cos(elevation) so the camera stays a constant distance from the target: a true orbit rather than a lurching one. The attractor itself never moves; only the viewpoint does.

### UI
This project puts more weight on interface than the earlier simulations: reusable `Slider` and `Button` structs, with the sliders cleanly separating their internal *position* (0–100) from the *parameter value* they represent (via a min/max range and a mapping), so a slider can drive any parameter range while its knob and hit-test stay in position space.

---

## Controls

- **Rotate sliders** orbit the camera around the attractor (horizontal = spin, vertical = tilt).
- **Rho / Sigma / Beta sliders** change the system parameters live; the attractor reshapes as you drag.
- **Reset** restores the classic parameters (σ=10, ρ=28, β=8/3).
- **Reset Trace** clears the accumulated trail.

---

## Building

```bash
mkdir build && cd build
cmake ..
make
./lorenz
```

Requires raylib (on the Pi, built from source with the OpenGL ES 2.0 backend).

---

## Possible extensions

- **Two-trajectory divergence.** Run two attractors from near-identical starting points in different colors; they track together, then split — a direct visualization of sensitive dependence on initial conditions.
- **Color the trail** by speed, height, or age for a gradient rather than a flat color.
- **Other attractors** (Rössler, Chua, Aizawa) on the same rendering/integration scaffold.