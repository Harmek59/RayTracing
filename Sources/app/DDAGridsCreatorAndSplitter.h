#pragma once

#include "GridDDA.h"

class DDAGridsCreatorAndSplitter {
public:
    DDAGridsCreatorAndSplitter() = default;

    std::tuple<GridDDA::GridData, std::vector<GridDDA::Cell>, std::vector<uint32_t>> calculateBaseGridAndCells(const TriangleMesh &mesh);

    std::pair<std::vector<GridDDA::GridData>, std::vector<GridDDA::Cell>>
    baseGridSplitter(GridDDA::GridData baseGrid, std::vector<GridDDA::Cell> cellsArr);

    inline static float numberOfTrianglesMultiplier = 10.f;
    inline static int maxModelsGridResolution = 512;
    inline static int sizeOfSubGrid = 32;
};