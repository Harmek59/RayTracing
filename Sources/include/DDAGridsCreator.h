#pragma once

#include "GridDDA.h"

class DDAGridsCreator {
public:
    DDAGridsCreator() = default;

    std::pair<GridDDA::GridData, std::vector<GridDDA::Cell>> calculateBaseGridAndCells(const TriangleMesh &mesh);

    std::pair<std::vector<GridDDA::GridData>, std::vector<GridDDA::Cell>>
    baseGridSplitter(GridDDA::GridData baseGrid, std::vector<GridDDA::Cell> cellsArr);
};