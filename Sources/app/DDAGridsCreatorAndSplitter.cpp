#include "DDAGridsCreatorAndSplitter.h"
#include <glm/gtx/string_cast.hpp>
#include <limits>
#include <algorithm>
#include <iterator>

uint32_t from3D(glm::ivec3 cords, glm::ivec3 gridResolution) {
    return (cords.z * gridResolution.x * gridResolution.y) + (cords.y * gridResolution.x) + cords.x;
};

std::tuple<GridDDA::GridData, std::vector<GridDDA::Cell>, std::vector<uint32_t>>
DDAGridsCreatorAndSplitter::calculateBaseGridAndCells(const TriangleMesh &mesh) {
    GridDDA::GridData gridData;
    std::vector<GridDDA::Cell> cellsArray;
    std::vector<uint32_t> trianglesIndicesArray;

    auto numberOfTriangles = mesh.getNumberOfTriangles();
    auto boundingBox = mesh.calculateBoundingBox();

    glm::vec3 bbSize = boundingBox.second - boundingBox.first;

    float cubeRoot = float(std::pow(float(numberOfTriangles) * numberOfTrianglesMultiplier / (bbSize.x * bbSize.y * bbSize.z),
                                    1.f / 3.f));

    auto &gridResolution = gridData.gridResolution;
    auto &cellSize = gridData.cellSize;

    gridResolution = glm::vec3(cubeRoot) * bbSize;

    gridResolution.x = std::clamp(gridResolution.x, 1, maxModelsGridResolution);
    gridResolution.y = std::clamp(gridResolution.y, 1, maxModelsGridResolution);
    gridResolution.z = std::clamp(gridResolution.z, 1, maxModelsGridResolution);

    cellSize = glm::vec3(bbSize.x / float(gridResolution.x), bbSize.y / float(gridResolution.y),
                         bbSize.z / float(gridResolution.z));

    std::vector<std::vector<uint32_t>> localIndicesArr(
            uint32_t(gridResolution.x * gridResolution.y * gridResolution.z));

    glm::vec3 cellHalfSize = glm::vec3(cellSize.x / 2., cellSize.y / 2., cellSize.z / 2.);


    for (auto iter = mesh.getBeginOfMeshTriangles(); iter < mesh.getEndOfMeshTriangles(); ++iter) {

        auto tBB = triangleBoundingBox(TriangleMesh::getTriangles()[iter]);

        glm::ivec3 tBoundingIndexFirst = glm::floor(
                (tBB.first - boundingBox.first) / bbSize * glm::vec3(gridResolution));
        glm::ivec3 tBoundingIndexSecond = glm::ceil(
                (tBB.second - boundingBox.first) / bbSize * glm::vec3(gridResolution));

        tBoundingIndexFirst = glm::clamp(tBoundingIndexFirst, glm::ivec3(0), gridResolution - glm::ivec3(1));
        tBoundingIndexSecond = glm::clamp(tBoundingIndexSecond, glm::ivec3(0), gridResolution - glm::ivec3(1));

        for (int z = tBoundingIndexFirst.z; z <= tBoundingIndexSecond.z; ++z) {
            for (int y = tBoundingIndexFirst.y; y <= tBoundingIndexSecond.y; ++y) {
                for (int x = tBoundingIndexFirst.x; x <= tBoundingIndexSecond.x; ++x) {
                    glm::vec3 cellCenter = boundingBox.first + cellHalfSize +
                                           glm::vec3(cellSize.x * x, cellSize.y * y, cellSize.z * z);

                    if (triangleBoxOverlap(cellCenter, cellHalfSize, TriangleMesh::getTriangles()[iter])) {
                        auto triangleIndex = iter;
                        localIndicesArr[from3D(glm::ivec3(x, y, z), gridResolution)].push_back(
                                uint32_t(triangleIndex));
                    }

                }
            }
        }
    }
    uint32_t numberOfCells = (gridResolution.x * gridResolution.y * gridResolution.z);
    size_t indiciesArrSize = 0;
    for (auto &v : localIndicesArr) indiciesArrSize += v.size();

    cellsArray.reserve(numberOfCells);
    trianglesIndicesArray.reserve(indiciesArrSize);

    uint32_t cellBeingIndex = 0;
    for (auto &v : localIndicesArr) {
        cellsArray.push_back(GridDDA::Cell{cellBeingIndex,
                                           cellBeingIndex + uint32_t(v.size())});
        cellBeingIndex += uint32_t(v.size());
        std::move(v.begin(), v.end(), std::back_inserter(trianglesIndicesArray));
    }

    gridData.gridBegin = boundingBox.first;
    gridData.gridEnd = boundingBox.second;
    gridData.cellsBeginIndex = uint32_t(std::distance(cellsArray.begin(), std::prev(cellsArray.end(), numberOfCells)));
    gridData.cellSize = cellSize;
    gridData.cellsEndIndex = uint32_t(std::distance(cellsArray.begin(), cellsArray.end()));
    gridData.gridResolution = gridResolution;

    return {gridData, cellsArray, trianglesIndicesArray};
}

std::pair<std::vector<GridDDA::GridData>, std::vector<GridDDA::Cell>>
DDAGridsCreatorAndSplitter::baseGridSplitter(GridDDA::GridData baseGrid, std::vector<GridDDA::Cell> baseCellsArr) {

    std::vector<GridDDA::GridData> gridDataArr;
    std::vector<GridDDA::Cell> cellsArr;

    glm::ivec3 numberOfSubGrids = glm::round(glm::vec3(baseGrid.gridResolution) / float(sizeOfSubGrid));
    numberOfSubGrids = glm::clamp(numberOfSubGrids, glm::ivec3(1), glm::ivec3(10000));

    glm::ivec3 sizeOfSubGrid = glm::round(glm::vec3(baseGrid.gridResolution) / glm::vec3(numberOfSubGrids));


    for (int i = 0; i < numberOfSubGrids.x; i++) {
        for (int j = 0; j < numberOfSubGrids.y; j++) {
            for (int k = 0; k < numberOfSubGrids.z; k++) {

                glm::ivec3 gridBegin = glm::ivec3(i, j, k) * sizeOfSubGrid;

                glm::ivec3 subGridEnd = sizeOfSubGrid + gridBegin;
                subGridEnd = glm::min(sizeOfSubGrid + gridBegin, baseGrid.gridResolution);
                if (i == numberOfSubGrids.x - 1) { //last
                    subGridEnd.x = std::max(subGridEnd.x, baseGrid.gridResolution.x);
                }
                if (j == numberOfSubGrids.y - 1) { //last
                    subGridEnd.y = std::max(subGridEnd.y, baseGrid.gridResolution.y);
                }
                if (k == numberOfSubGrids.z - 1) { //last
                    subGridEnd.z = std::max(subGridEnd.z, baseGrid.gridResolution.z);
                }

                GridDDA::GridData gData;
                gData.cellSize = baseGrid.cellSize;

                glm::ivec3 minGridBB = glm::ivec3(std::numeric_limits<int>::max());
                glm::ivec3 maxGridBB = glm::ivec3(-1);

                for (int z = gridBegin.z; z < subGridEnd.z; z++) {
                    for (int y = gridBegin.y; y < subGridEnd.y; y++) {
                        for (int x = gridBegin.x; x < subGridEnd.x; x++) {
                            GridDDA::Cell cell = baseCellsArr[from3D(glm::ivec3(x, y, z), baseGrid.gridResolution)];
                            if (cell.beginOfTrianglesIndicesArray <
                                cell.endOfTrianglesIndicesArray) { //cell contain something
                                maxGridBB.x = std::max(maxGridBB.x, x);
                                maxGridBB.y = std::max(maxGridBB.y, y);
                                maxGridBB.z = std::max(maxGridBB.z, z);

                                minGridBB.x = std::min(minGridBB.x, x);
                                minGridBB.y = std::min(minGridBB.y, y);
                                minGridBB.z = std::min(minGridBB.z, z);
                            }
                        }
                    }
                }

                if (minGridBB.x >= maxGridBB.x || minGridBB.y >= maxGridBB.y ||
                    minGridBB.z >= maxGridBB.z) { //empty grid
                    continue;
                }
                gData.cellsBeginIndex = uint32_t(cellsArr.size());
                for (int z = minGridBB.z; z <= maxGridBB.z; z++) {
                    for (int y = minGridBB.y; y <= maxGridBB.y; y++) {
                        for (int x = minGridBB.x; x <= maxGridBB.x; x++) {
                            GridDDA::Cell cell = baseCellsArr[from3D(glm::ivec3(x, y, z), baseGrid.gridResolution)];
                            cellsArr.push_back(cell);
                        }
                    }
                }
                gData.cellsEndIndex = uint32_t(cellsArr.size());


                gData.gridBegin = glm::vec3(minGridBB) * gData.cellSize + baseGrid.gridBegin;
                gData.gridEnd = glm::vec3(maxGridBB + glm::ivec3(1)) * baseGrid.cellSize +
                                baseGrid.gridBegin;
                gData.gridResolution = maxGridBB + glm::ivec3(1) - minGridBB;


                gridDataArr.push_back(std::move(gData));
                gridBegin += sizeOfSubGrid;
            }
        }
    }


    return {gridDataArr, cellsArr};
}