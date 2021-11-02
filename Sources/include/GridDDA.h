#pragma once

#include <cmath>
#include <algorithm>
#include <iterator>
#include <cmath>
#include "TriangleMesh.h"
#include "tribox3.h"

#include <iostream>
#include <cstring>

#include <glm/gtx/string_cast.hpp>

std::pair<glm::vec3, glm::vec3> triangleBoundingBox(const TriangleMesh::Triangle &t);

class GridDDA {
public:
    struct Cell {
        uint32_t beginOfTrianglesIndiciesArray;
        uint32_t endOfTrianglesIndiciesArray;
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

    GridDDA(const TriangleMesh &mesh);

    static void
    bindBuffers(uint32_t trianglesIndiciesBufferBinding, uint32_t cellsBufferBinding, uint32_t gridDataBinding) {
        std::cout << "\nBinding buffers: indiceist array: " << trianglesIndiciesArray.size() << ", cellsArray: "
                  << cellsArray.size()
                  << ", gridDataArray: " << gridDataArray.size() << std::endl;

        bindOneBuffer(trianglesIndiciesBuffer, trianglesIndiciesArray, trianglesIndiciesBufferBinding);
        bindOneBuffer(cellsBuffer, cellsArray, cellsBufferBinding);
        bindOneBuffer(gridDataBuffer, gridDataArray, gridDataBinding);
    }

    int getNumberOfCells() const {
        return int(std::distance(beginOfCellsArray, endOfCellsArray));
    }

    GridData getGridData() const {
        return gridData;
    }

//private:
    glm::ivec3 to3D(uint32_t idx) {
        int z = idx / (gridResolution.x * gridResolution.y);
        idx -= (z * gridResolution.x * gridResolution.y);
        int y = idx / gridResolution.x;
        int x = idx % gridResolution.x;
        return {x, y, z};
    }

    uint32_t from3D(glm::ivec3 cords) {
        return (cords.z * gridResolution.x * gridResolution.y) + (cords.y * gridResolution.x) + cords.x;
    }


    inline static std::vector<uint32_t> trianglesIndiciesArray;
    inline static std::vector<Cell> cellsArray;
    inline static std::vector<GridData> gridDataArray;

    inline static Buffer trianglesIndiciesBuffer;
    inline static Buffer cellsBuffer;
    inline static Buffer gridDataBuffer;

    template<typename dataType>
    static void bindOneBuffer(Buffer &buff, dataType &data, uint32_t bindingBlock) {
        using Type = typename dataType::value_type;
        buff = Buffer(data.size() * sizeof(Type), GL_DYNAMIC_DRAW);
        buff.bind(GL_SHADER_STORAGE_BUFFER);
        Type *dataPtr = (Type *) buff.mapBuffer(GL_SHADER_STORAGE_BUFFER,
                                                GL_WRITE_ONLY | GL_MAP_INVALIDATE_BUFFER_BIT);
        if (!dataPtr) {
            throw std::runtime_error("GridDDA error during update function: mapping buffer failed");
        }
        std::memcpy(dataPtr, data.data(), data.size() * sizeof(Type));

        buff.unMap(GL_SHADER_STORAGE_BUFFER);
        buff.unBind();

        buff.bindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingBlock);
    }

    std::vector<Cell>::iterator beginOfCellsArray;
    std::vector<Cell>::iterator endOfCellsArray;

    glm::ivec3 gridResolution;
    glm::vec3 cellSize;

    GridData gridData;

    int gridDataIndex;
};

