#include "GridDDA.h"

#include <iostream>
#include <cmath>
#include <algorithm>
#include <iterator>
#include <cmath>

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

GridDDA::GridDDA(GridData gridData) {
    gridData.cellsBeginIndex += uint32_t(cellsArray.size());
    gridData.cellsEndIndex += uint32_t(cellsArray.size());
    gridDataArray.push_back(gridData);
    gridDataIndex = int(gridDataArray.size() - 1);
}

void GridDDA::addToCellArrAndTrianglesIndicesArr(const std::vector<Cell>& cellsArr, const std::vector<uint32_t> &trianglesIndArr){
    std::transform(cellsArr.begin(), cellsArr.end(), std::back_inserter(cellsArray),[](auto cell){
        cell.beginOfTrianglesIndicesArray += uint32_t(trianglesIndicesArray.size());
        cell.endOfTrianglesIndicesArray += uint32_t(trianglesIndicesArray.size());
        return std::move(cell);
    });
    std::move(trianglesIndArr.begin(), trianglesIndArr.end(), std::back_inserter(trianglesIndicesArray));
}

void GridDDA::bindBuffers(uint32_t trianglesIndiciesBufferBinding, uint32_t cellsBufferBinding, uint32_t gridDataBinding) {
    trianglesIndicesBuffer.createShaderStorageBuffer(trianglesIndicesArray, trianglesIndiciesBufferBinding);
    cellsBuffer.createShaderStorageBuffer(cellsArray, cellsBufferBinding);
    gridDataBuffer.createShaderStorageBuffer(gridDataArray, gridDataBinding);
}

std::vector<GridDDA::GridData> &GridDDA::getGridDataArray() {
    return gridDataArray;
}

int GridDDA::getGridDataIndex() const{
    return gridDataIndex;
}

void GridDDA::clear(){
    cellsArray.clear();
    trianglesIndicesArray.clear();
    gridDataArray.clear();
}

