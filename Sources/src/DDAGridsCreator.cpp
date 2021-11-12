#include "DDAGridsCreator.h"
#include <glm/gtx/string_cast.hpp>


//TODO w siatkach towrza sie dziury bo bb gridow sa za male

uint32_t from3D(glm::ivec3 cords, glm::ivec3 gridResolution) {
    return (cords.z * gridResolution.x * gridResolution.y) + (cords.y * gridResolution.x) + cords.x;
};

std::pair<GridDDA::GridData, std::vector<GridDDA::Cell>>
DDAGridsCreator::calculateBaseGridAndCells(const TriangleMesh &mesh) {
    GridDDA::GridData gridData;
    std::vector<GridDDA::Cell> cellsArray;

    auto numberOfTriangles = mesh.getNumberOfTriangles();
    auto boundingBox = mesh.calculateBoundingBox();


    glm::vec3 bbSize = boundingBox.second - boundingBox.first;

    float cubeRoot = float(std::pow(float(numberOfTriangles) * 10.0 / (bbSize.x * bbSize.y * bbSize.z),
                                    1.f / 3.f));    //TODO jakos ogranac liczbe przez ktora mnoze

    auto &gridResolution = gridData.gridResolution;
    auto &cellSize = gridData.cellSize;

    gridResolution = glm::vec3(cubeRoot) * bbSize;

    gridResolution.x = std::clamp(gridResolution.x, 1, 512);
    gridResolution.y = std::clamp(gridResolution.y, 1, 512);
    gridResolution.z = std::clamp(gridResolution.z, 1, 512);

    cellSize = glm::vec3(bbSize.x / float(gridResolution.x), bbSize.y / float(gridResolution.y),
                         bbSize.z / float(gridResolution.z));

    std::vector<std::vector<uint32_t>> localIndiciesArr(
            uint32_t(gridResolution.x * gridResolution.y * gridResolution.z));
    //std::vector<Cell> localCellsArray(gridResolution.x * gridResolution.y * gridResolution.z);

    glm::vec3 cellHalfSize = glm::vec3(cellSize.x / 2., cellSize.y / 2., cellSize.z / 2.);


    for (auto iter = mesh.beginOfMeshTriangles; iter < mesh.endOfMeshTriangles; ++iter) {

        auto tBB = triangleBoundingBox(TriangleMesh::triangles[iter]);

        glm::ivec3 tBoundingIndexFirst = glm::floor(
                (tBB.first - boundingBox.first) / bbSize * glm::vec3(gridResolution));
        tBoundingIndexFirst -= glm::ivec3(1);   //TODO find out why I have to do it (e.g. FinalBaseMesh.obj)
        glm::ivec3 tBoundingIndexSecond = glm::ceil(
                (tBB.second - boundingBox.first) / bbSize * glm::vec3(gridResolution));
        //tBoundingIndexSecond += glm::ivec3(1);

        tBoundingIndexFirst = glm::clamp(tBoundingIndexFirst, glm::ivec3(0), gridResolution - glm::ivec3(1));
        tBoundingIndexSecond = glm::clamp(tBoundingIndexSecond, glm::ivec3(0), gridResolution - glm::ivec3(1));

        for (int z = tBoundingIndexFirst.z; z <= tBoundingIndexSecond.z; ++z) {
            for (int y = tBoundingIndexFirst.y; y <= tBoundingIndexSecond.y; ++y) {
                for (int x = tBoundingIndexFirst.x; x <= tBoundingIndexSecond.x; ++x) {


                    glm::vec3 cellCenter = boundingBox.first + cellHalfSize +
                                           glm::vec3(cellSize.x * x, cellSize.y * y, cellSize.z * z);

                    // !!! przy liczeniu triangle-box overlap pamietaj zeby dodac jakis offset aby trojkaty równolegle do boku boxa, które są "na" tej ścianie nie byly odrzucane. W triangleBoxOverlap jest przyklad
                    if (triangleBoxOverlap(cellCenter, cellHalfSize, TriangleMesh::triangles[iter])) {
                        auto triangleIndex = iter;
                        localIndiciesArr[from3D(glm::ivec3(x, y, z), gridResolution)].push_back(
                                uint32_t(triangleIndex));
                    }

                }
            }
        }
    }
    uint32_t numberOfCells = (gridResolution.x * gridResolution.y * gridResolution.z);
    size_t indiciesArrSize = 0;
    for (auto &v : localIndiciesArr) indiciesArrSize += v.size();

    cellsArray.reserve(numberOfCells);
    GridDDA::trianglesIndiciesArray.reserve(GridDDA::trianglesIndiciesArray.size() + indiciesArrSize);

    for (auto &v : localIndiciesArr) {
        cellsArray.push_back(GridDDA::Cell{uint32_t(GridDDA::trianglesIndiciesArray.size()),
                                           uint32_t(GridDDA::trianglesIndiciesArray.size() + v.size())});
        std::move(v.begin(), v.end(), std::back_inserter(GridDDA::trianglesIndiciesArray));


    }

    gridData.gridBegin = boundingBox.first;
    gridData.gridEnd = boundingBox.second;
    gridData.cellsBeginIndex = uint32_t(std::distance(cellsArray.begin(), std::prev(cellsArray.end(), numberOfCells)));
    gridData.cellSize = cellSize;
    gridData.cellsEndIndex = uint32_t(std::distance(cellsArray.begin(), cellsArray.end()));
    gridData.gridResolution = gridResolution;

    return {gridData, cellsArray};
}

std::pair<std::vector<GridDDA::GridData>, std::vector<GridDDA::Cell>>
DDAGridsCreator::baseGridSplitter(GridDDA::GridData baseGrid, std::vector<GridDDA::Cell> baseCellsArr) {

//    std::cout << "Original: res=" << glm::to_string(baseGrid.gridResolution) << ", begin="
//              << glm::to_string(baseGrid.gridBegin) << ", end="
//              << glm::to_string(baseGrid.gridEnd) << ", begIndx=" << baseGrid.cellsBeginIndex
//              << ", endIndx=" << baseGrid.cellsEndIndex << std::endl;

    std::vector<GridDDA::GridData> gridDataArr;
    std::vector<GridDDA::Cell> cellsArr;

    glm::ivec3 numberOfSubGrids = glm::round(glm::vec3(baseGrid.gridResolution) / 32.f);
    numberOfSubGrids = glm::clamp(numberOfSubGrids, glm::ivec3(1), glm::ivec3(10000));

//    std::cout << "NumberOfSubGrids: " << glm::to_string(numberOfSubGrids) << std::endl;

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
//                std::cout << "BeforeSplit: gridBegin = " << glm::to_string(gridBegin) << " subEnd = "
//                          << glm::to_string(subGridEnd) << std::endl;

                GridDDA::GridData gData;
                gData.cellSize = baseGrid.cellSize;

                glm::ivec3 minGridBB = glm::ivec3(99999); // TODO change to max int
                glm::ivec3 maxGridBB = glm::ivec3(0);

                for (int z = gridBegin.z; z < subGridEnd.z; z++) {
                    for (int y = gridBegin.y; y < subGridEnd.y; y++) {
                        for (int x = gridBegin.x; x < subGridEnd.x; x++) {
                            GridDDA::Cell cell = baseCellsArr[from3D(glm::ivec3(x, y, z), baseGrid.gridResolution)];
                            if (cell.beginOfTrianglesIndiciesArray <
                                cell.endOfTrianglesIndiciesArray) { //cell contain something
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

//                std::cout << "Local BB: " << glm::to_string(minGridBB) << " | " << glm::to_string(maxGridBB)
//                          << std::endl;

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

//                std::cout << "SPLIT: res=" << glm::to_string(gridDataArr.back().gridResolution) << ", begin="
//                          << glm::to_string(gridDataArr.back().gridBegin) << ", end="
//                          << glm::to_string(gridDataArr.back().gridEnd) << ", begIndx="
//                          << gridDataArr.back().cellsBeginIndex
//                          << ", endIndx=" << gridDataArr.back().cellsEndIndex << std::endl;

                gridBegin += sizeOfSubGrid;
            }
        }
    }


    return {gridDataArr, cellsArr};
}