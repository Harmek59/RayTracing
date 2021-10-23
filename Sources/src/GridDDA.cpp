#include "GridDDA.h"

std::pair<glm::vec3, glm::vec3> triangleBoundingBox(const TriangleMesh::Triangle &t) {
    auto &vert1 = TriangleMesh::getVertices()[t.vertexIndex1];
    auto &vert2 = TriangleMesh::getVertices()[t.vertexIndex2];
    auto &vert3 = TriangleMesh::getVertices()[t.vertexIndex3];

    float minX = std::min(vert1.x, std::min(vert2.x, vert3.x));
    float minY = std::min(vert1.y, std::min(vert2.y, vert3.y));
    float minZ = std::min(vert1.z, std::min(vert2.z, vert3.z));

    float maxX = std::max(vert1.x, std::max(vert2.x, vert3.x));
    float maxY = std::max(vert1.y, std::max(vert2.y, vert3.y));
    float maxZ = std::max(vert1.z, std::max(vert2.z, vert3.z));

    return std::make_pair(glm::vec3(minX, minY, minZ), glm::vec3(maxX, maxY, maxZ));
}

GridDDA::GridDDA(const TriangleMesh &mesh) {
    auto numberOfTriangles = mesh.getNumberOfTriangles();
    auto boundingBox = mesh.calculateBoundingBox();


    glm::vec3 bbSize = boundingBox.second - boundingBox.first;

    float cubeRoot = float(std::pow(float(numberOfTriangles) * 10.0 / (bbSize.x * bbSize.y * bbSize.z), 1.f / 3.f));

    gridResolution = glm::vec3(cubeRoot) * bbSize;

    gridResolution.x = std::clamp(gridResolution.x, 1, 128);
    gridResolution.y = std::clamp(gridResolution.y, 1, 128);
    gridResolution.z = std::clamp(gridResolution.z, 1, 128);

    cellSize = glm::vec3(bbSize.x / float(gridResolution.x), bbSize.y / float(gridResolution.y),
                         bbSize.z / float(gridResolution.z));

    std::vector<std::vector<uint32_t>> localIndiciesArr(
            uint32_t(gridResolution.x * gridResolution.y * gridResolution.z));
    //std::vector<Cell> localCellsArray(gridResolution.x * gridResolution.y * gridResolution.z);

    glm::vec3 cellHalfSize = glm::vec3(cellSize.x / 2., cellSize.y / 2., cellSize.z / 2.);


    for (auto iter = mesh.beginOfMeshTriangles; iter < mesh.endOfMeshTriangles; ++iter) {

        auto tBB = triangleBoundingBox(*iter);

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

                    float boxcenter[3] = {cellCenter.x, cellCenter.y, cellCenter.z};
                    float boxhalfsize[3] = {cellHalfSize.x, cellHalfSize.y, cellHalfSize.z};
                    auto &vert1 = TriangleMesh::getVertices()[(*iter).vertexIndex1];
                    auto &vert2 = TriangleMesh::getVertices()[(*iter).vertexIndex2];
                    auto &vert3 = TriangleMesh::getVertices()[(*iter).vertexIndex3];
                    float triverts[3][3] = {{vert1.x, vert1.y, vert1.z},
                                            {vert2.x, vert2.y, vert2.z},
                                            {vert3.x, vert3.y, vert3.z}};


                    if (triBoxOverlap(boxcenter, boxhalfsize, triverts)) {
                        auto triangleIndex = std::distance(mesh.triangles.begin(), iter);
                        localIndiciesArr[from3D(glm::ivec3(x, y, z))].push_back(uint32_t(triangleIndex));
                    }

                }
            }
        }
    }
    uint32_t numberOfCells = (gridResolution.x * gridResolution.y * gridResolution.z);
    size_t indiciesArrSize = 0;
    for (auto &v : localIndiciesArr) indiciesArrSize += v.size();

//    std::cout << "DDA Grid report:\nNumberOfCells:\t" << numberOfCells << "\n number of indicies:\t"
//    << indiciesArrSize
//    << "\nGridResolution:\t" << gridResolution.x << ", " << gridResolution.y
//    << ", " << gridResolution.z <<
//    "\nCellSize:\t" << cellSize.x << ", " << cellSize.y << ", " << cellSize.z << std::endl;

    cellsArray.reserve(cellsArray.size() + numberOfCells);
    trianglesIndiciesArray.reserve(trianglesIndiciesArray.size() + indiciesArrSize);

    for (auto &v : localIndiciesArr) {
        cellsArray.push_back(Cell{uint32_t(trianglesIndiciesArray.size()),
                                  uint32_t(trianglesIndiciesArray.size() + v.size())});
        std::move(v.begin(), v.end(), std::back_inserter(trianglesIndiciesArray));


    }

    beginOfCellsArray = std::prev(cellsArray.end(), numberOfCells);
    endOfCellsArray = cellsArray.end();


    gridData.gridBegin = boundingBox.first;
    gridData.gridEnd = boundingBox.second;
    gridData.cellsBeginIndex = uint32_t(std::distance(cellsArray.begin(), beginOfCellsArray));
    gridData.cellSize = cellSize;
    gridData.cellsEndIndex = uint32_t(std::distance(cellsArray.begin(), endOfCellsArray));
    gridData.gridResolution = gridResolution;


    gridDataArray.push_back(gridData);
}