#include "raylib.h"
#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <random>

using namespace std;

bool inBounds(int r, int c, int ROWS, int COLS) {
    return r >= 0 && r < ROWS && c >= 0 && c < COLS;
}

// Reset button
struct button {

    string value;
    Color c = DARKGRAY;
    Color onhover = GRAY;

    void draw (Rectangle r) {

        DrawRectangleRec(r, c);
        
        DrawText(value.c_str(), r.x+15, r.y+10, 20, RAYWHITE);

    }

    void onhoverdraw (Rectangle r) {

        DrawRectangleRec(r, onhover);

        DrawText(value.c_str(), r.x+15, r.y+10, 20, RAYWHITE);
    }

};

// Program main entry point
int main(void)
{
    // Initialization
    
    const int screenWidth = 800;
    const int screenHeight = 400;

    InitWindow(screenWidth, screenHeight, "Falling Sand");

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second


    // Size of pixel
    const int CELL = 2;
    const int COLS = screenWidth / CELL;
    const int ROWS = screenHeight / CELL;
    
    // A fixed grid, all cells exist, each is just "is there sand here?"
    vector<vector<bool>> grid(ROWS, vector<bool>(COLS, false));

    // double lastTime = GetTime();

    Vector2 coords;

    int BRUSH = 4;   // radius in cells


    button reset {"Reset"};
    Rectangle resetRect = { 10, 10, 175, 30 };
    bool resetHover = false;

    button inc {"+"};
    Rectangle incRect = { 10, 70, 40, 40 };
    bool incHover = false;

    button dec {"-"};
    Rectangle decRect = { 55, 70, 40, 40 };
    bool decHover = false;

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        Vector2 coords = GetMousePosition();
        resetHover = CheckCollisionPointRec(coords, resetRect);
        incHover = CheckCollisionPointRec(coords, incRect);
        decHover = CheckCollisionPointRec(coords, decRect);


        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(coords, resetRect)) {
            grid = vector<vector<bool>>(ROWS, vector<bool>(COLS, false));
        } else if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(coords, incRect)) {
            if (BRUSH < 20) BRUSH++;
        } else if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(coords, decRect)) {
            if (BRUSH > 1) BRUSH --;
        }

        bool overUI = CheckCollisionPointRec(coords, resetRect)
           || CheckCollisionPointRec(coords, incRect)
           || CheckCollisionPointRec(coords, decRect);

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && !overUI) {
            int cc = (int)(coords.x / CELL);   // center column
            int cr = (int)(coords.y / CELL);   // center row

            for (int dr = -BRUSH; dr <= BRUSH; dr++) {
                for (int dc = -BRUSH; dc <= BRUSH; dc++) {
                    if (dr*dr + dc*dc <= BRUSH*BRUSH) {          // inside the circle
                        int r = cr + dr;
                        int c = cc + dc;
                        if (r >= 0 && r < ROWS && c >= 0 && c < COLS) {   // bounds check!
                            grid[r][c] = true;
                        }
                    }
                }
            }
        }

        // TODO: Update your variables here

        // ===== UPDATE: falling (bottom-to-top!) =====
        for (int r = ROWS - 1; r >= 0; r--) {
            for (int c = 0; c < COLS; c++) {
                if (!grid[r][c]) continue;
                if (inBounds(r + 1, c, ROWS, COLS) && !grid[r+1][c]) {
                    grid[r+1][c] = true;
                    grid[r][c] = false;
                } else {
                    bool rightFirst = GetRandomValue(0, 1);
                    int first  = rightFirst ? c + 1 : c - 1;
                    int second = rightFirst ? c - 1 : c + 1;

                    if (inBounds(r+1, first, ROWS, COLS) && !grid[r+1][first]) {
                        grid[r+1][first] = true;  grid[r][c] = false;
                    } else if (inBounds(r+1, second, ROWS, COLS) && !grid[r+1][second]) {
                        grid[r+1][second] = true; grid[r][c] = false;   // ← the fallback
                    }
                }
            }
        }

        // Draw
        
        BeginDrawing();

            ClearBackground(BLACK);
            
            for (int r = 0; r < ROWS; r++) {          // direction doesn't matter for drawing
                for (int c = 0; c < COLS; c++) {
                    if (grid[r][c]) {
                        DrawRectangle(c * CELL, r * CELL, CELL, CELL, WHITE);
                    }
                }
            }
            if (resetHover) {
                reset.onhoverdraw(resetRect);
            } else {
                reset.draw(resetRect);
            }

            if (incHover) {
                inc.onhoverdraw(incRect);
            } else {
                inc.draw(incRect);
            }

            if (decHover) {
                dec.onhoverdraw(decRect);
            } else {
                dec.draw(decRect);
            }
            
            string pixelRadius = "Pixel Radius: " + to_string(BRUSH);
            DrawText(pixelRadius.c_str(), 10, 45, 20, WHITE);

        EndDrawing();
        
    }

    // De-Initialization
    
    CloseWindow();        // Close window and OpenGL context

    return 0;
}
