#pragma once

#include <cmath>
#include <algorithm>
#include <iterator>
#include "TriangleMesh.h"
#include "tribox3.h"

#include <iostream>

std::pair<glm::vec3, glm::vec3> triangleBoundingBox(const TriangleMesh::Triangle& t) {
    auto& vert1 = TriangleMesh::getVertices()[t.vertexIndex1];
    auto& vert2 = TriangleMesh::getVertices()[t.vertexIndex2];
    auto& vert3 = TriangleMesh::getVertices()[t.vertexIndex3];

    float minX = std::min(vert1.x, std::min(vert2.x, vert3.x));
    float minY = std::min(vert1.y, std::min(vert2.y, vert3.y));
    float minZ = std::min(vert1.z, std::min(vert2.z, vert3.z));

    float maxX = std::max(vert1.x, std::max(vert2.x, vert3.x));
    float maxY = std::max(vert1.y, std::max(vert2.y, vert3.y));
    float maxZ = std::max(vert1.z, std::max(vert2.z, vert3.z));

    return std::make_pair(glm::vec3(minX, minY, minZ), glm::vec3(maxX, maxY, maxZ));
}

class GridDDA {
public:
    struct Cell {
        uint32_t beginOfTrianglesIndiciesArray;
        uint32_t endOfTrianglesIndiciesArray;
        uint32_t : 32;
        uint32_t : 32;
    };

    struct GridData {
        glm::vec3 gridBegin;
        uint32_t cellsBeginIndex;
        glm::vec3 gridEnd;
        uint32_t cellsEndIndex;
        glm::ivec3 gridResolution;
        uint32_t : 32;
        glm::vec3 cellSize;
        uint32_t : 32;
    };

    struct TrianglesIndicies{
        uint32_t index;
        glm::vec3 offset;
    };

    GridDDA(const TriangleMesh &mesh) {
        int numberOfTriangles = mesh.getNumberOfTriangles();
        auto boundingBox = mesh.calculateBoundingBox();


        glm::vec3 bbSize = boundingBox.second - boundingBox.first;

        float cubeRoot = std::powf(numberOfTriangles / (bbSize.x * bbSize.y * bbSize.z), 1.f / 3.f);

        gridResolution = glm::vec3(cubeRoot) * bbSize;

        gridResolution.x = std::clamp(gridResolution.x, 1, 128);
        gridResolution.y = std::clamp(gridResolution.y, 1, 128);
        gridResolution.z = std::clamp(gridResolution.z, 1, 128);

        cellSize = glm::vec3(bbSize.x / gridResolution.x, bbSize.y / gridResolution.y, bbSize.z / gridResolution.z);

        std::vector<std::vector<TrianglesIndicies>> localIndiciesArr(gridResolution.x * gridResolution.y * gridResolution.z);
        //std::vector<Cell> localCellsArray(gridResolution.x * gridResolution.y * gridResolution.z);

        glm::vec3 cellHalfSize = glm::vec3(cellSize.x / 2., cellSize.y / 2., cellSize.z / 2.);



        for (auto iter = mesh.beginOfMeshTriangles; iter < mesh.endOfMeshTriangles; ++iter) {

            auto tBB = triangleBoundingBox(*iter);

            glm::ivec3 tBoundingIndexFirst = glm::floor((tBB.first - boundingBox.first) / bbSize * glm::vec3(gridResolution));
            tBoundingIndexFirst -= glm::ivec3(1);   //TODO find out why I have to do it (e.g. FinalBaseMesh.obj)
            glm::ivec3 tBoundingIndexSecond = glm::ceil((tBB.second - boundingBox.first) / bbSize * glm::vec3(gridResolution));
            //tBoundingIndexSecond += glm::ivec3(1);

            tBoundingIndexFirst = glm::clamp(tBoundingIndexFirst, glm::ivec3(0), gridResolution - glm::ivec3(1));
            tBoundingIndexSecond = glm::clamp(tBoundingIndexSecond, glm::ivec3(0), gridResolution - glm::ivec3(1));

            for (int z = tBoundingIndexFirst.z; z <= tBoundingIndexSecond.z; ++z) {
                for (int y = tBoundingIndexFirst.y; y <= tBoundingIndexSecond.y; ++y) {
                    for (int x = tBoundingIndexFirst.x; x <= tBoundingIndexSecond.x; ++x) {


                        glm::vec3 cellCenter = boundingBox.first + cellHalfSize +
                                glm::vec3(cellSize.x * x, cellSize.y * y, cellSize.z * z);

                        float boxcenter[3] = { cellCenter.x, cellCenter.y, cellCenter.z };
                        float boxhalfsize[3] = { cellHalfSize.x, cellHalfSize.y, cellHalfSize.z };
                        auto& vert1 = TriangleMesh::getVertices()[(*iter).vertexIndex1];
                        auto& vert2 = TriangleMesh::getVertices()[(*iter).vertexIndex2];
                        auto& vert3 = TriangleMesh::getVertices()[(*iter).vertexIndex3];
                        float triverts[3][3] = { {vert1.x, vert1.y, vert1.z},
                                                 {vert2.x, vert2.y, vert2.z},
                                                 {vert3.x, vert3.y, vert3.z} };


                        if (triBoxOverlap(boxcenter, boxhalfsize, triverts)) {
                            auto triangleIndex = std::distance(mesh.triangles.begin(), iter);
                            if (from3D(glm::ivec3(x, y, z)) >= localIndiciesArr.size()) {
                                std::cout <<"error: "<< from3D(glm::ivec3(x, y, z)) << " | " << localIndiciesArr.size() << std::endl;
                                std::cout << x << ", " << y << ", " << z << std::endl;

                                std::cout << "======\n";
                            }
                            else
                                localIndiciesArr[from3D(glm::ivec3(x, y, z))].push_back(TrianglesIndicies{ uint32_t(triangleIndex), glm::vec3(0.0) });
                        }

                    }
                }
            }
        }
        uint32_t numberOfCells = (gridResolution.x * gridResolution.y * gridResolution.z);
        uint32_t indiciesArrSize = 0;
        for (auto& v : localIndiciesArr)  indiciesArrSize += v.size();

        std::cout << "DDA Grid report:\nNumberOfCells:\t" << numberOfCells << "\n number of indicies:\t"
        << indiciesArrSize
        << "\nGridResolution:\t" << gridResolution.x << ", " << gridResolution.y
        << ", " << gridResolution.z <<
        "\nCellSize:\t" << cellSize.x << ", " << cellSize.y << ", " << cellSize.z << std::endl;

        cellsArray.reserve(cellsArray.size() + numberOfCells);
        trianglesIndiciesArray.reserve(trianglesIndiciesArray.size() + indiciesArrSize);

        for (auto& v : localIndiciesArr) {
            cellsArray.push_back(Cell{ trianglesIndiciesArray.size(),
                                       trianglesIndiciesArray.size() + v.size() });
            std::move(v.begin(), v.end(), std::back_inserter(trianglesIndiciesArray));


        }

        beginOfCellsArray = std::prev(cellsArray.end(), numberOfCells);
        endOfCellsArray = cellsArray.end();


        GridData gridData;
        gridData.gridBegin = boundingBox.first;
        gridData.gridEnd = boundingBox.second;
        gridData.cellsBeginIndex = std::distance(cellsArray.begin(), beginOfCellsArray);
        gridData.cellSize = cellSize;
        gridData.cellsEndIndex = std::distance(cellsArray.begin(), endOfCellsArray);
        gridData.gridResolution = gridResolution;


        gridDataArray.push_back(std::move(gridData));


    }

    static void
    bindBuffers(uint32_t trianglesIndiciesBufferBinding, uint32_t cellsBufferBinding, uint32_t gridDataBinding) {
        auto bindBuffer = [](Buffer &buff, auto &data, uint32_t bindingBlock) {
            using Type = std::decay_t<decltype(data)>::value_type;
            buff = Buffer(data.size() * sizeof(Type), GL_DYNAMIC_DRAW);
            buff.bind(GL_SHADER_STORAGE_BUFFER);
            Type *dataPtr = (Type *) buff.mapBuffer(GL_SHADER_STORAGE_BUFFER,
                                                    GL_WRITE_ONLY | GL_MAP_INVALIDATE_BUFFER_BIT);
            if (!dataPtr) {
                throw std::exception("Temp_Spheres error during update function: mapping buffer failed");
            }
            std::memcpy(dataPtr, data.data(), data.size() * sizeof(Type));

            buff.unMap(GL_SHADER_STORAGE_BUFFER);
            buff.unBind();

            buff.bindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingBlock);
        };
        std::cout << "\nBinding buffers: indiceist array: " << trianglesIndiciesArray.size() << ", cellsArray: "
                  << cellsArray.size()
                  << ", gridDataArray: " << gridDataArray.size() << std::endl;

        bindBuffer(trianglesIndiciesBuffer, trianglesIndiciesArray, trianglesIndiciesBufferBinding);
        bindBuffer(cellsBuffer, cellsArray, cellsBufferBinding);
        bindBuffer(gridDataBuffer, gridDataArray, gridDataBinding);


        std::ofstream tIA{"triangleIndiciesArray.txt"};
        int counter = 0;
        for (auto i: trianglesIndiciesArray) {
            tIA << counter++ << ": " << i.index << "\n";
        }
        tIA << "=================================\n";
        std::ofstream ca{"cellsArray.txt"};
        counter = 0;
        for (auto &c: cellsArray) {
            ca << counter++ << ": " << c.beginOfTrianglesIndiciesArray << ", " << c.endOfTrianglesIndiciesArray << "\n";
        }
        ca << "=================================\n";
        std::ofstream gDA{"gridDataArray.txt"};
        counter = 0;
        for (auto &g: gridDataArray) {
            gDA << counter++ << ": cells[" << g.cellsBeginIndex << ", " << g.cellsEndIndex << "]\n";
        }
        gDA << "=================================\n";

    }

private:
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


    inline static std::vector<TrianglesIndicies> trianglesIndiciesArray;
    inline static std::vector<Cell> cellsArray;
    inline static std::vector<GridData> gridDataArray;

    inline static Buffer trianglesIndiciesBuffer;
    inline static Buffer cellsBuffer;
    inline static Buffer gridDataBuffer;

    std::vector<Cell>::iterator beginOfCellsArray;
    std::vector<Cell>::iterator endOfCellsArray;

    glm::ivec3 gridResolution;
    glm::vec3 cellSize;
};

