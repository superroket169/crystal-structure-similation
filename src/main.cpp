#include "raylib.h"
#include "raymath.h"
#include <iostream>
#include <string>
#include <stdlib.h>
#include <vector>

enum Mode { CLOSE_PACKED, STICKED_MODEL, NONE_MODEL };

struct Bond {
    Vector3 start;
    Vector3 end;
};

int main(int argc, char* argv[])
{
    Mode prog_mode = STICKED_MODEL;
    
    const int screenWidth = 1000;
    const int screenHeight = 800;

    InitWindow(screenWidth, screenHeight, "Crystal Structure Simulation");

    Camera3D camera = { 0 };
    camera.position = (Vector3){ 15.0f, 15.0f, 15.0f };
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    DisableCursor();
    SetTargetFPS(60);

    int gridSize;
    float spacing;
    
    if (prog_mode == STICKED_MODEL) {
        gridSize = 15;
        spacing = 1.8f;
    } else if (prog_mode == CLOSE_PACKED) {
        gridSize = 8;
        spacing = 0.65f;
    } else return 1;

    float offset = (gridSize - 1) * spacing / 2.0f;
    float bondThickness = 0.06f;
    
    std::vector<Matrix> naTransforms;
    std::vector<Matrix> clTransforms;
    std::vector<Bond> bonds;

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

                if ((x + y + z) % 2 == 0) {
                    Matrix transform = MatrixMultiply(MatrixScale(0.25f, 0.25f, 0.25f), MatrixTranslate(pos.x, pos.y, pos.z));
                    naTransforms.push_back(transform); 
                }
                else {
                    Matrix transform = MatrixMultiply(MatrixScale(0.45f, 0.45f, 0.45f), MatrixTranslate(pos.x, pos.y, pos.z));
                    clTransforms.push_back(transform);
                }
            
                // X axis
                if (x < gridSize - 1) {
                    Vector3 nextX = { (x + 1) * spacing - offset, y * spacing - offset, z * spacing - offset };
                    bonds.push_back({pos, nextX});
                }
            
                // Y axis
                if (y < gridSize - 1) {
                    Vector3 nextY = { x * spacing - offset, (y + 1) * spacing - offset, z * spacing - offset };
                    bonds.push_back({pos, nextY});
                }
            
                // Z axis
                if (z < gridSize - 1) {
                    Vector3 nextZ = { x * spacing - offset, y * spacing - offset, (z + 1) * spacing - offset };
                    bonds.push_back({pos, nextZ});
                }
            }
        }
    }

    Model naModel = LoadModelFromMesh(GenMeshSphere(1.0f, 16, 16));
    naModel.materials[0].maps[MATERIAL_MAP_ALBEDO].color = PURPLE;

    Model clModel = LoadModelFromMesh(GenMeshSphere(1.0f, 16, 16));
    clModel.materials[0].maps[MATERIAL_MAP_ALBEDO].color = GOLD;

    while (!WindowShouldClose())
    {
        UpdateCamera(&camera, CAMERA_FIRST_PERSON);
        
        float verticalSpeed = 5.0f * GetFrameTime();
        
        if (IsKeyDown(KEY_LEFT_SHIFT)) {
            camera.position.y += verticalSpeed;
            camera.target.y += verticalSpeed;
        }
        if (IsKeyDown(KEY_LEFT_CONTROL)) {
            camera.position.y -= verticalSpeed;
            camera.target.y -= verticalSpeed;
        }

        BeginDrawing();
            ClearBackground(RAYWHITE);

            BeginMode3D(camera);
                
                for (const Matrix& mat : naTransforms) {
                    naModel.transform = mat;
                    DrawModel(naModel, Vector3Zero(), 1.0f, WHITE); 
                }

                for (const Matrix& mat : clTransforms) {
                    clModel.transform = mat;
                    DrawModel(clModel, Vector3Zero(), 1.0f, WHITE);
                }

                for (const auto& bond : bonds) {
                    DrawCylinderEx(bond.start, bond.end, bondThickness, bondThickness, 8, LIGHTGRAY);
                }

            EndMode3D();

            DrawText("NaCl", 10, 10, 20, DARKGRAY);
            DrawText("Sodyum (Na): Purple", 10, 40, 20, PURPLE);
            DrawText("Klor (Cl) : Yellow", 10, 70, 20, GOLD);
            DrawFPS(10, 100);
        
        EndDrawing();
    }

    EnableCursor();
    CloseWindow();
    return 0;
}
