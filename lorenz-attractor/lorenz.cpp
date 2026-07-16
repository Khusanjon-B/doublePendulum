#include "raylib.h"
#include <vector>
#include <cmath>
#include <string>
#include <algorithm>

using namespace std;

// ============================================================
//  Helpers
// ============================================================

// Map a value from one range to another (normalize, then rescale).
float mapRange(float value, float inMin, float inMax, float outMin, float outMax) {
    float t = (value - inMin) / (inMax - inMin);   // 0..1
    return outMin + t * (outMax - outMin);
}

// ============================================================
//  Slider
//  - `val` is ALWAYS the position along the track, 0..100.
//  - The represented PARAMETER value is derived via value(),
//    using this slider's own [minVal, maxVal] range.
// ============================================================
struct Slider {
    string label;

    Vector2 pos;    // top-left corner
    Vector2 size;   // .x = width, .y = height (raylib convention)

    int val = 50;   // POSITION only: 0..100

    // Parameter range this slider maps onto (position 0->minVal, 100->maxVal)
    float minVal = 0.0f;
    float maxVal = 100.0f;

    Color c = RAYWHITE;   // outline
    Color f = GRAY;       // fill
    Color k = SKYBLUE;    // knob

    int   padd = 2;
    float knobRadius = 12.0f;
    bool  vert = false;

    // The actual parameter value this slider represents.
    float value() {
        return mapRange((float)val, 0.0f, 100.0f, minVal, maxVal);
    }

    // Set the slider's POSITION so that value() == v (inverse map).
    void setFromValue(float v) {
        val = (int)mapRange(v, minVal, maxVal, 0.0f, 100.0f);
    }

    void draw() {
        float t = val / 100.0f;

        if (!vert) {
            DrawRectangleV(pos, size, c);
            DrawRectangleV(
                { pos.x + padd, pos.y + padd },
                { (size.x - 2 * padd) * t, size.y - 2 * padd },
                f);
            DrawCircleV({ pos.x + t * size.x, pos.y + size.y / 2 }, knobRadius, k);
        } else {
            DrawRectangleV(pos, size, c);
            float fillHeight = (size.y - 2 * padd) * t;
            DrawRectangleV(
                { pos.x + padd, pos.y + (size.y - padd) - fillHeight },
                { size.x - 2 * padd, fillHeight },
                f);
            DrawCircleV({ pos.x + size.x / 2, pos.y + (1.0f - t) * size.y }, knobRadius, k);
        }
    }

    bool knobContains(Vector2 point) {
        float t = val / 100.0f;
        Vector2 knob = (!vert)
            ? Vector2{ pos.x + t * size.x, pos.y + size.y / 2 }
            : Vector2{ pos.x + size.x / 2, pos.y + (1.0f - t) * size.y };
        return CheckCollisionPointCircle(point, knob, knobRadius);
    }

    // Drag: set POSITION from the mouse, always clamped to 0..100.
    void dragTo(Vector2 mouse) {
        float t = vert
            ? 1.0f - (mouse.y - pos.y) / size.y
            :        (mouse.x - pos.x) / size.x;
        val = max(0, min(100, (int)(t * 100)));
    }
};

// ============================================================
//  Button
// ============================================================
struct Button {
    string value;
    Color c = DARKGRAY;
    Color onhover = GRAY;

    void draw(Rectangle r, bool hover = false) {
        DrawRectangleRec(r, hover ? onhover : c);
        DrawText(value.c_str(), r.x + 5, r.y + 5, 20, RAYWHITE);
    }
};

// ============================================================
//  Lorenz system
// ============================================================
struct State {
    float x, y, z;
};

State derivatives(State s, float sigma, float rho, float beta) {
    return {
        sigma * (s.y - s.x),
        s.x * (rho - s.z) - s.y,
        s.x * s.y - beta * s.z
    };
}

State step(State s, State d, float h) {
    return { s.x + d.x * h, s.y + d.y * h, s.z + d.z * h };
}

// ============================================================
//  Main
// ============================================================
int main(void)
{
    const int screenWidth = 800;
    const int screenHeight = 400;
    InitWindow(screenWidth, screenHeight, "Lorenz Attractor");
    SetTargetFPS(60);

    // ---- Camera ----
    Camera3D camera = { 0 };
    camera.position   = { 22.0f, 22.0f, 22.0f };
    camera.target     = { 0.0f, 0.0f, 0.0f };
    camera.up         = { 0.0f, 1.0f, 0.0f };
    camera.fovy       = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    float phi = 0.0f, theta = 0.0f;
    float radius = 35.0f;

    // ---- Default (ideal) parameters — the saved values ----
    const float SIGMA_DEFAULT = 10.0f;
    const float RHO_DEFAULT   = 28.0f;
    const float BETA_DEFAULT  = 8.0f / 3.0f;

    // ---- Camera-rotation sliders ----
    Slider rotatex;
    rotatex.label = "Rotate XY";
    rotatex.pos = { 10, 380 };
    rotatex.size = { 300, 10 };

    Slider rotatez;
    rotatez.label = "Rotate Z";
    rotatez.pos = { 20, 30 };
    rotatez.size = { 10, 300 };
    rotatez.vert = true;

    // ---- Parameter sliders (each carries its own value range) ----
    Slider rhoS;
    rhoS.label = "Rho";
    rhoS.pos = { 650, 110 };
    rhoS.size = { 100, 10 };
    rhoS.minVal = 0.0f;  rhoS.maxVal = 50.0f;
    rhoS.setFromValue(RHO_DEFAULT);       // position so value() == 28

    Slider sigmaS;
    sigmaS.label = "Sigma";
    sigmaS.pos = { 650, 155 };
    sigmaS.size = { 100, 10 };
    sigmaS.minVal = 0.0f;  sigmaS.maxVal = 20.0f;
    sigmaS.setFromValue(SIGMA_DEFAULT);

    Slider betaS;
    betaS.label = "Beta";
    betaS.pos = { 650, 200 };
    betaS.size = { 100, 10 };
    betaS.minVal = 0.0f;  betaS.maxVal = 5.0f;
    betaS.setFromValue(BETA_DEFAULT);

    // drag flags
    bool dragRotX = false, dragRotZ = false;
    bool dragRho = false, dragSigma = false, dragBeta = false;

    // ---- Buttons ----
    Button reset { "Reset" };
    Rectangle resetRect = { 650, 10, 175, 30 };

    Button resetTrace { "Reset Trace" };
    Rectangle resetTraceRect = { 650, 50, 175, 30 };

    // ---- Lorenz state + trail ----
    State lorenz = { 1.0f, 1.0f, 1.0f };
    vector<Vector3> trail;
    const float SCALE = 0.5f;

    // ============================================================
    //  Main loop
    // ============================================================
    while (!WindowShouldClose())
    {
        // ---------- INPUT ----------
        Vector2 coords = GetMousePosition();

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            // Buttons first
            if (CheckCollisionPointRec(coords, resetRect)) {
                rhoS.setFromValue(RHO_DEFAULT);
                sigmaS.setFromValue(SIGMA_DEFAULT);
                betaS.setFromValue(BETA_DEFAULT);
            } else if (CheckCollisionPointRec(coords, resetTraceRect)) {
                trail.clear();
            }
            // Then sliders (grab-once)
            else if (rotatex.knobContains(coords)) dragRotX  = true;
            else if (rotatez.knobContains(coords)) dragRotZ  = true;
            else if (rhoS.knobContains(coords))    dragRho   = true;
            else if (sigmaS.knobContains(coords))  dragSigma = true;
            else if (betaS.knobContains(coords))   dragBeta  = true;
        }

        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            dragRotX = dragRotZ = dragRho = dragSigma = dragBeta = false;
        }

        if (dragRotX)  rotatex.dragTo(coords);
        if (dragRotZ)  rotatez.dragTo(coords);
        if (dragRho)   rhoS.dragTo(coords);
        if (dragSigma) sigmaS.dragTo(coords);
        if (dragBeta)  betaS.dragTo(coords);

        // ---------- CAMERA ----------
        theta = rotatex.val / 100.0f * 2.0f * PI;
        phi   = (rotatez.val / 100.0f - 0.5f) * (PI * 2.0f / 3.0f);
        camera.position.x = radius * cos(phi) * cos(theta);
        camera.position.z = radius * cos(phi) * sin(theta);
        camera.position.y = radius * sin(phi);
        camera.target = { 0, 0, 0 };

        // ---------- LORENZ (mapped parameter values) ----------
        float sigma = sigmaS.value();
        float rho   = rhoS.value();
        float beta  = betaS.value();

        float dt = 0.01f;
        State k1 = derivatives(lorenz, sigma, rho, beta);
        State k2 = derivatives(step(lorenz, k1, dt/2.0f), sigma, rho, beta);
        State k3 = derivatives(step(lorenz, k2, dt/2.0f), sigma, rho, beta);
        State k4 = derivatives(step(lorenz, k3, dt),      sigma, rho, beta);
        lorenz = step(lorenz, step(step(step(k1, k2, 2.0f), k3, 2.0f), k4, 1.0f), dt/6.0f);

        // axis-swap (sim z -> world y), center (-27), scale
        trail.push_back({ lorenz.x * SCALE, (lorenz.z - 27.0f) * SCALE, lorenz.y * SCALE });

        // ---------- DRAW ----------
        BeginDrawing();
            ClearBackground(BLACK);

            BeginMode3D(camera);
                DrawGrid(20, 1.0f);
                for (size_t i = 1; i < trail.size(); i++)
                    DrawLine3D(trail[i-1], trail[i], SKYBLUE);
            EndMode3D();

            // Camera sliders
            rotatex.draw();
            DrawText("Rotate XY", rotatex.pos.x, rotatex.pos.y - 25, 20, WHITE);
            rotatez.draw();
            DrawText("Rotate Z", rotatez.pos.x, rotatez.pos.y - 25, 20, WHITE);

            // Buttons
            reset.draw(resetRect);
            resetTrace.draw(resetTraceRect);

            // Parameter sliders — display the MAPPED value, not the position
            rhoS.draw();
            DrawText(TextFormat("Rho = %.1f", rho), rhoS.pos.x, rhoS.pos.y - 25, 20, WHITE);
            sigmaS.draw();
            DrawText(TextFormat("Sigma = %.1f", sigma), sigmaS.pos.x, sigmaS.pos.y - 25, 20, WHITE);
            betaS.draw();
            DrawText(TextFormat("Beta = %.2f", beta), betaS.pos.x, betaS.pos.y - 25, 20, WHITE);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}