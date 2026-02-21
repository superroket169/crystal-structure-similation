#include "raylib.h"
#include <iostream>
#include <string>

enum Mode { CLOSE_PACKED, STICKED_MODEL, NONE_MODEL };

int main(int argc, char* argv[])
{
    Mode prog_mode = NONE_MODEL;

    for(int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];

        if (arg == "--mode" && i+1 < argc)
        {
            std::string nextArg = argv[++i];

            if (nextArg == "closePack")
                prog_mode = CLOSE_PACKED;
            else if (nextArg == "stickPack")
                prog_mode = STICKED_MODEL;
            else
            {
                std::cerr << "no mode selected\n";
                return 1;
            }
        }
    }

    const int screenWidth = 1000;
    const int screenHeight = 800;

    InitWindow(screenWidth, screenHeight, "Crystal Structure Simulation");

    // Kamera
    Camera3D camera = { 0 };
    camera.position = (Vector3){ 8.0f, 8.0f, 8.0f };
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    DisableCursor();
    SetTargetFPS(60);

    int gridSize;
    float spacing;
    
    if (prog_mode == STICKED_MODEL)
    {
        gridSize = 6;
        spacing = 1.8f;
    }
    else if (prog_mode == CLOSE_PACKED)
    {
        gridSize = 8;
        spacing = 0.65f;
    }
    else return 1;

    float offset = (gridSize - 1) * spacing / 2.0f;
    float bondThickness = 0.06f; 

    while (!WindowShouldClose())
    {
        UpdateCamera(&camera, CAMERA_FIRST_PERSON);
        
        float verticalSpeed = 5.0f * GetFrameTime();
        
        if (IsKeyDown(KEY_LEFT_SHIFT))
        {
            camera.position.y += verticalSpeed;
            camera.target.y += verticalSpeed;
        }
        if (IsKeyDown(KEY_LEFT_CONTROL))
        {
            camera.position.y -= verticalSpeed;
            camera.target.y -= verticalSpeed;
        }

        BeginDrawing();
            ClearBackground(RAYWHITE);

            BeginMode3D(camera);

                for (int x = 0; x < gridSize; x++)
                {
                    for (int y = 0; y < gridSize; y++)
                    {
                        for (int z = 0; z < gridSize; z++)
                        {
                            
                            Vector3 pos = {
                                x * spacing - offset,
                                y * spacing - offset,
                                z * spacing - offset
                            };

                            if ((x + y + z) % 2 == 0) DrawSphere(pos, 0.25f, PURPLE);
                            else DrawSphere(pos, 0.45f, GOLD);
                            
                            // X axis
                            if (x < gridSize - 1)
                            {
                                Vector3 nextX = { (x + 1) * spacing - offset, y * spacing - offset, z * spacing - offset };
                                DrawCylinderEx(pos, nextX, bondThickness, bondThickness, 8, LIGHTGRAY);
                            }
                            
                            // Y axis
                            if (y < gridSize - 1)
                            {
                                Vector3 nextY = { x * spacing - offset, (y + 1) * spacing - offset, z * spacing - offset };
                                DrawCylinderEx(pos, nextY, bondThickness, bondThickness, 8, LIGHTGRAY);
                            }
                            
                            // Z axis
                            if (z < gridSize - 1)
                            {
                                Vector3 nextZ = { x * spacing - offset, y * spacing - offset, (z + 1) * spacing - offset };
                                DrawCylinderEx(pos, nextZ, bondThickness, bondThickness, 8, LIGHTGRAY);
                            }
                        }
                    }
                }

                // DrawGrid(10, 1.0f);

            EndMode3D();

            DrawText("NaCl", 10, 10, 20, DARKGRAY);
            DrawText("Sodyum (Na): Purple", 10, 40, 20, PURPLE);
            DrawText("Klor (Cl) : Yellow", 10, 70, 20, GOLD);
            DrawText("Turn structure by mouse", 10, 100, 10, GRAY);
        
        EndDrawing();
    }

    EnableCursor();
    CloseWindow();
    return 0;
}
