#ifndef CRYSTAL_GENERATOR_H
#define CRYSTAL_GENERATOR_H

#include <string>
#include <vector>

namespace Generator {

    
    enum Mode { CLOSE_PACKED, STICKED_MODEL, NONE_MODEL };
    enum BravaisMode { CUBIC, TETRAGONAL, ORTHORHOMBIC, MONOCLINIC };

    struct Bond {
        Vector3 start;
        Vector3 end;
    };

    struct CrystalLattice {
        float a, b, c;
        float alpha, beta, gamma;


    }

    class Crystal {
    public:
        Mode prog_mode = STICKED_MODEL;
        BravaisMode bravais_mode = CUBIC;
        int gridSize = 6;
    
        bool isAngleCustom = true;
        bool twoType = true;
        bool isSizeCustom = false;

        std::string name;

        CrystalLattice crystal_settings =
        {
            a = 1.8,
            b = 3.0,
            c = 4.0
        }
    
    private:
        
        Crystal() = default;
        Crystal();
        ~Crystal();
   
        //-------SETTERS-------
        
        CrystalLattice& setCrystal() { return crystal_settings; }
        void setBravis(BravaisMode m) { bravais_mode = m; }

        // VISUAL GENERATON FN
    
        void generate() {

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
        }


    };
    
    Crystal::Crystal() {
    }
    
    Crystal::~Crystal() {
    }

}


#endif // !CRYSTAL_GENERATOR_H
