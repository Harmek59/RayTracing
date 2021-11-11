#include "TriangleMesh.h"

#include <set>
#include <algorithm>
#include <iterator>
#include <cassert>
#include <cstring>

#include <iostream>
#include <stdexcept>

TriangleMesh::TriangleMesh(std::vector<Vertex> v, std::vector<Triangle> t, std::vector<Normals> n,
                           std::vector<TextureCoords> tc) {
    auto sizeOfV = v.size();
    auto sizeOfT = t.size();
    auto sizeOfN = n.size();
    auto sizeOfTC = tc.size();

    beginOfMeshTriangles = uint32_t(triangles.size());
    beginOfMeshVertices = uint32_t(vertices.size());
    uint32_t verticesOffset = uint32_t(vertices.size());
    uint32_t normalsOffset = uint32_t(normals.size());
    uint32_t texCoordsOffset = uint32_t(textureCoords.size());
    std::transform(t.begin(), t.end(), std::back_inserter(triangles),
                   [verticesOffset, normalsOffset, texCoordsOffset](auto &trg) {
                       trg.vertexIndex1 += verticesOffset;
                       trg.vertexIndex2 += verticesOffset;
                       trg.vertexIndex3 += verticesOffset;

                       trg.normalIndex1 += normalsOffset;
                       trg.normalIndex2 += normalsOffset;
                       trg.normalIndex3 += normalsOffset;

                       return std::move(trg);
                   });

    std::move(v.begin(), v.end(), std::back_inserter(vertices));
    std::move(n.begin(), n.end(), std::back_inserter(normals));
    std::move(tc.begin(), tc.end(), std::back_inserter(textureCoords));

    endOfMeshTriangles = uint32_t(triangles.size());
    endOfMeshVertices = uint32_t(vertices.size());


//    std::cout << "NumberOfTriangle:\t" << beginOfMeshTriangles <<" | "<< endOfMeshTriangles << std::endl;
//    std::cout << "NumberOfVertices:\t" << std::distance(beginOfMeshVertices, endOfMeshVertices) << std::endl;
}

void TriangleMesh::normalize() {
    auto bBox = calculateBoundingBox();
    float maxDimensionSize = std::max({(bBox.second - bBox.first).x, (bBox.second - bBox.first).y,
                                       (bBox.second - bBox.first).z});

    for (auto i = beginOfMeshVertices; i < endOfMeshVertices; ++i) {
        TriangleMesh::vertices[i].x = (TriangleMesh::vertices[i].x - bBox.first.x) / maxDimensionSize;
        TriangleMesh::vertices[i].y = (TriangleMesh::vertices[i].y - bBox.first.y) / maxDimensionSize;
        TriangleMesh::vertices[i].z = (TriangleMesh::vertices[i].z - bBox.first.z) / maxDimensionSize;
        TriangleMesh::vertices[i].x -= (bBox.second.x - bBox.first.x) / maxDimensionSize / 2.f;
        TriangleMesh::vertices[i].y -= (bBox.second.y - bBox.first.y) / maxDimensionSize / 2.f;
        TriangleMesh::vertices[i].z -= (bBox.second.z - bBox.first.z) / maxDimensionSize / 2.f;
    }

    std::tie(bbBegin, bbEnd) = calculateBoundingBox();  //TODO calculate normalized  bb simmilar to the way above
}

std::pair<glm::vec3, glm::vec3> TriangleMesh::calculateBoundingBox() const {
    float minX, minY, minZ;
    minX = minY = minZ = 99999.; // TODO change to maxFloatVal
    float maxX, maxY, maxZ;
    maxX = maxY = maxZ = -99999.; // TODO change to minFloatVal;

    for (auto i = beginOfMeshVertices; i < endOfMeshVertices; i++) {
        const auto &vert = vertices[i];
        ///std::cout << "=================\nVertex1: " << vert.x << ", " << vert.y << ", " << vert.z << '\n';
        maxX = std::max(maxX, vert.x);
        minX = std::min(minX, vert.x);
        maxY = std::max(maxY, vert.y);
        minY = std::min(minY, vert.y);
        maxZ = std::max(maxZ, vert.z);
        minZ = std::min(minZ, vert.z);
    }

    return std::make_pair(glm::vec3{minX, minY, minZ}, glm::vec3{maxX, maxY, maxZ});
}

void TriangleMesh::bindBuffers(uint32_t trianglesBufferBinding, uint32_t verticesBufferBinding,
                               uint32_t normalsBufferBinding, uint32_t textureCoordsBufferBinding) {
    auto bindBuffer = [](Buffer &buff, auto &data, uint32_t bindingBlock) {
        using Type = typename std::decay_t<decltype(data)>::value_type;
        if(data.empty()){
            return;
        }
        buff = Buffer(data.size() * sizeof(Type), GL_DYNAMIC_DRAW);
        buff.bind(GL_SHADER_STORAGE_BUFFER);
        Type *dataPtr = (Type *) buff.mapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY | GL_MAP_INVALIDATE_BUFFER_BIT);
        if (!dataPtr) {
            throw std::runtime_error("Temp_Spheres error during update function: mapping buffer failed");
        }
        std::memcpy(dataPtr, data.data(), data.size() * sizeof(Type));

        buff.unMap(GL_SHADER_STORAGE_BUFFER);
        buff.unBind();

        buff.bindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingBlock);
    };
//    std::cout << "\nBinding buffers: trianglesArray: " << triangles.size() << ", verticesArray: " << vertices.size()
//              << std::endl;
    bindBuffer(trianglesBuffer, triangles, trianglesBufferBinding);
    bindBuffer(verticesBuffer, vertices, verticesBufferBinding);
    bindBuffer(normalsBuffer, normals, normalsBufferBinding);
    bindBuffer(textureCoordsBuffer, textureCoords, textureCoordsBufferBinding);
}