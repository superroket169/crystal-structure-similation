#include "raylib.h"
#include "raymath.h"
#include <iostream>
#include <string>
#include <stdlib.h>
#include <vector>

enum Mode { CLOSE_PACKED, STICKED_MODEL, NONE_MODEL };
enum BravaisMode { CUBIC, TETRAGONAL, ORTHORHOMBIC, MONOCLINIC };

struct Bond {
    Vector3 start;
    Vector3 end;
};

int main(int argc, char* argv[])
{
    Mode prog_mode = STICKED_MODEL;
    BravaisMode bravais_mode = CUBIC;
    int gridSize = 6;
    float a1 = 1.8f, a2 = 3.0f, a3 = 4.0f; // kenar uzunlukları
    float a12 = 0.0f, a23 = 0.0f, a31 = 0.0f; // açılar.
    
    bool isAngleCustom = true;
    bool twoType = true;
    bool isSizeCustom = false;

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];

        if (arg == "--stickpacked")      prog_mode = STICKED_MODEL;
        else if (arg == "--closepacked") prog_mode = CLOSE_PACKED;
        else if (arg == "--twotype")     twoType = true;
        else if (arg == "--onetype")     twoType = false;
        else if (arg == "--size") {
            if (i + 1 < argc) {
                gridSize = std::stoi(argv[++i]);
                isSizeCustom = true;
            }
            else {
                std::cerr << "Hata: --size flag'i 1 adet argüman almalı." << std::endl;
                return 1;
            }
        }
        else if (arg == "--setbravis") {
            if (i + 1 < argc) {
                std::string mode = argv[++i];

                if (mode == "cubic")      bravais_mode = CUBIC; // cubic
                else if (mode == "tetr")  bravais_mode = TETRAGONAL; // tetragonal
                else if (mode == "orth")  bravais_mode = ORTHORHOMBIC; // orthorhombic
                else if (mode == "mono")  bravais_mode = MONOCLINIC; // monoclinic
                else {
                    std::cerr << "Hata: Bilinmeyen bravais modu (" << mode << ")." << std::endl;
                    return 1;
                }
            }
            else {
                std::cerr << "Hata: --setbravis flag'i 1 adet argüman almalı." << std::endl;
                return 1;
            }
        }


        else if (arg == "--sticks") {
            if (bravais_mode == CUBIC) {
                if (i + 1 < argc) {
                    a1 = std::stof(argv[++i]);
                    a2 = a1; 
                    a3 = a1;
                } else {
                    std::cerr << "Hata: Cubic modunda --sticks sadece 1 argüman (a1) alır." << std::endl;
                    return 1;
                }
            }
            else if (bravais_mode == TETRAGONAL) {
                if (i + 2 < argc) {
                    a1 = std::stof(argv[++i]);
                    a3 = a1;
                    a2 = std::stof(argv[++i]);
                }
                else {
                    std::cerr << "Hata: Tetragonal modunda --sticks 2 argüman almalıdır." << std::endl;
                    return 1;
                }
            }
            else if (bravais_mode == ORTHORHOMBIC) {
                if (i + 3 < argc) {
                    a1 = std::stof(argv[++i]);
                    a3 = std::stof(argv[++i]);
                    a2 = std::stof(argv[++i]);
                }
                else {
                    std::cerr << "Hata: Orthorhombic modunda --sticks 3 argüman almalıdır." << std::endl;
                    return 1;
                }
            }
            else if (bravais_mode == MONOCLINIC) {
                if (i + 3 < argc) {
                    a1 = std::stof(argv[++i]);
                    a3 = std::stof(argv[++i]);
                    a2 = std::stof(argv[++i]);
                    if (isAngleCustom) { a23 = 1.0f; isAngleCustom = false; } 
                }
                else {
                    std::cerr << "Hata: Orthorhombic modunda --sticks 3 argüman almalıdır." << std::endl;
                    return 1;
                }
            }
        }


        else if (arg == "--angles") {
            if (bravais_mode == CUBIC || bravais_mode == TETRAGONAL || bravais_mode == ORTHORHOMBIC) {
                std::cerr << "Hata: Cubic, Orthorhombic ve Tetragonal modları --angles flag'ini almaz." << std::endl;
                return 1;
            }
            else if (bravais_mode == MONOCLINIC) {
                if (i + 1 < argc) {
                    isAngleCustom = false;
                    a23 = std::stof(argv[++i]);
                }
                else {
                    std::cerr << "Hata: Orthorhombic modunda --angles 1 argüman almalıdır." << std::endl;
                    return 1;
                }
            }
        }
        else {
            std::cerr << "Hata: Bilinmeyen flag (" << arg << ")." << std::endl;
            return 1;
        }
    }

    if (prog_mode == CLOSE_PACKED && !isSizeCustom) {
        gridSize = 8;
        a1 = a2 = a3 = 0.65f;
    }
    
    // ---- Window Swttings ---- 
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

    float offsetX = (gridSize - 1) * a1 / 2.0f;
    float offsetY = (gridSize - 1) * a2 / 2.0f;
    float offsetZ = (gridSize - 1) * a3 / 2.0f;
    float bondThickness = 0.06f;
    
    auto getAtomPos = [&](int px, int py, int pz) -> Vector3 {
        return {
            px * a1 - offsetX + a12 * py + a23 * pz,
            py * a2 - offsetY + a31 * px,
            pz * a3 - offsetZ
        };
    };

    std::vector<Matrix> firstAtomTransforms;
    std::vector<Matrix> secondAtomTransforms;
    std::vector<Bond> bonds;

    for (int x = 0; x < gridSize; x++)
    {
        for (int y = 0; y < gridSize; y++)
        {
            for (int z = 0; z < gridSize; z++)
            {
                Vector3 pos = getAtomPos(x, y, z);

                if (twoType) {
                    if ((x + y + z) % 2 == 0) {
                        Matrix transform = MatrixMultiply(MatrixScale(0.25f, 0.25f, 0.25f), MatrixTranslate(pos.x, pos.y, pos.z));
                        firstAtomTransforms.push_back(transform); 
                    }
                    else {
                        Matrix transform = MatrixMultiply(MatrixScale(0.45f, 0.45f, 0.45f), MatrixTranslate(pos.x, pos.y, pos.z));
                        secondAtomTransforms.push_back(transform);
                    }
                }
                else {
                    Matrix transform = MatrixMultiply(MatrixScale(0.35f, 0.35f, 0.35f), MatrixTranslate(pos.x, pos.y, pos.z));
                    firstAtomTransforms.push_back(transform);
                }

                // X 
                if (x < gridSize - 1) {
                    bonds.push_back({pos, getAtomPos(x + 1, y, z)});
                }
                // Y 
                if (y < gridSize - 1) {
                    bonds.push_back({pos, getAtomPos(x, y + 1, z)});
                }
                // Z 
                if (z < gridSize - 1) {
                    bonds.push_back({pos, getAtomPos(x, y, z + 1)});
                }
            }
        }
    }

    Model firstAtomModel = LoadModelFromMesh(GenMeshSphere(1.0f, 16, 16));
    firstAtomModel.materials[0].maps[MATERIAL_MAP_ALBEDO].color = PURPLE;

    Model secondAtomModel = LoadModelFromMesh(GenMeshSphere(1.0f, 16, 16));
    secondAtomModel.materials[0].maps[MATERIAL_MAP_ALBEDO].color = GOLD;

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
                
                for (const Matrix& mat : firstAtomTransforms) {
                    firstAtomModel.transform = mat;
                    DrawModel(firstAtomModel, Vector3Zero(), 1.0f, WHITE); 
                }

                if (twoType) {
                    for (const Matrix& mat : secondAtomTransforms) {
                        secondAtomModel.transform = mat;
                        DrawModel(secondAtomModel, Vector3Zero(), 1.0f, WHITE);
                    }
                }

                if (prog_mode == STICKED_MODEL) {
                    for (const auto& bond : bonds) {
                        DrawCylinderEx(bond.start, bond.end, bondThickness, bondThickness, 8, LIGHTGRAY);
                    }
                }

            EndMode3D();

            if (twoType) {
                DrawText("Multi-Type Model (NaCl style)", 10, 10, 20, DARKGRAY);
                DrawText("Atom 1 : Purple", 10, 40, 20, PURPLE);
                DrawText("Atom 2 : Yellow", 10, 70, 20, GOLD);
            } else {
                DrawText("One-Type Model", 10, 10, 20, DARKGRAY);
                DrawText("Atom 1 : Purple", 10, 40, 20, PURPLE);
            }

            DrawFPS(10, 100);
        EndDrawing();
    }

    EnableCursor();
    CloseWindow();
    return 0;
}
