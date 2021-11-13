#pragma once

#include "GridDDA.h"

class DDAGridsCreator {
public:
    DDAGridsCreator() = default;

    std::pair<GridDDA::GridData, std::vector<GridDDA::Cell>> calculateBaseGridAndCells(const TriangleMesh &mesh);

    std::pair<std::vector<GridDDA::GridData>, std::vector<GridDDA::Cell>>
    baseGridSplitter(GridDDA::GridData baseGrid, std::vector<GridDDA::Cell> cellsArr);

    inline static float numberOfTrianglesMultiplier = 10.f;
    inline static int maxModelsGridResolution = 512;
    inline static int sizeOfSubGrid = 32;
};