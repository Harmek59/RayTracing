#pragma once

#include "TriangleMesh.h"
#include "tribox3.h"

std::pair<glm::vec3, glm::vec3> triangleBoundingBox(const TriangleMesh::Triangle &t);

class GridDDA {
public:
    struct Cell {
        uint32_t beginOfTrianglesIndicesArray;
        uint32_t endOfTrianglesIndicesArray;
    };

    struct GridData {
        glm::vec3 gridBegin;
        uint32_t cellsBeginIndex;
        glm::vec3 gridEnd;
        uint32_t cellsEndIndex;
        glm::ivec3 gridResolution;
        uint32_t modelDataId;
        glm::vec3 cellSize;
        uint32_t : 32;
    };

    GridDDA() = default;

    GridDDA(GridData gridData);

    static void
    addToCellArrAndTrianglesIndicesArr(const std::vector<Cell> &cellsArr, const std::vector<uint32_t> &trianglesIndArr);

    static void
    bindBuffers(uint32_t trianglesIndiciesBufferBinding, uint32_t cellsBufferBinding, uint32_t gridDataBinding);

    static std::vector<GridData> &getGridDataArray();

    int getGridDataIndex() const;

    static void clear();

private:
    inline static std::vector<uint32_t> trianglesIndicesArray;
    inline static std::vector<Cell> cellsArray;
    inline static std::vector<GridData> gridDataArray;

    inline static Buffer trianglesIndicesBuffer;
    inline static Buffer cellsBuffer;
    inline static Buffer gridDataBuffer;

    int gridDataIndex;
};

