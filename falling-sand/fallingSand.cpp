#include "raylib.h"
#include <iostream>
#include <vector>
#include <cmath>
#include <string>

using namespace std;

bool inBounds(int r, int c, int ROWS, int COLS) {
    return r >= 0 && r < ROWS && c >= 0 && c < COLS;
}

// Program main entry point
int main(void)
{
    // Initialization
    
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second


    // Size of pixel
    const int CELL = 5;
    const int COLS = screenWidth / CELL;
    const int ROWS = screenHeight / CELL;
    
    // A fixed grid, all cells exist, each is just "is there sand here?"
    vector<vector<bool>> grid(ROWS, vector<bool>(COLS, false));

    // double lastTime = GetTime();

    Vector2 coords;

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        
        const int BRUSH = 4;   // radius in cells

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            Vector2 coords = GetMousePosition();
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

        // Draw
        
        BeginDrawing();

            ClearBackground(BLACK);
            // Rows
            for (int r = 0; r < ROWS; r++) {
                int y = r * CELL;

                // Columns
                for (int c = 0; c < COLS; c++) { 
                        int x = c * CELL;
                        if (grid[r][c]) {
                            
                            DrawRectangle(x, y, CELL, CELL, WHITE);
                        }
                        
                }
            }
        EndDrawing();
        
    }

    // De-Initialization
    
    CloseWindow();        // Close window and OpenGL context

    return 0;
}
