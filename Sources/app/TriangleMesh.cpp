#include "TriangleMesh.h"

#include <algorithm>
#include <iterator>
#include <limits>
#include <iostream>
#include <stdexcept>

TriangleMesh::TriangleMesh(std::vector<Vertex> v, std::vector<Triangle> t, std::vector<Normals> n) {
    beginOfMeshTriangles = uint32_t(triangles.size());
    beginOfMeshVertices = uint32_t(vertices.size());
    uint32_t verticesOffset = uint32_t(vertices.size());
    uint32_t normalsOffset = uint32_t(normals.size());
    std::transform(t.begin(), t.end(), std::back_inserter(triangles),
                   [verticesOffset, normalsOffset](auto &trg) {
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

    endOfMeshTriangles = uint32_t(triangles.size());
    endOfMeshVertices = uint32_t(vertices.size());
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

    std::tie(bbBegin, bbEnd) = calculateBoundingBox();
}

std::pair<glm::vec3, glm::vec3> TriangleMesh::calculateBoundingBox() const {
    float minX, minY, minZ;
    minX = minY = minZ = std::numeric_limits<float>::max();
    float maxX, maxY, maxZ;
    maxX = maxY = maxZ = std::numeric_limits<float>::lowest();

    for (auto i = beginOfMeshVertices; i < endOfMeshVertices; i++) {
        const auto &vert = vertices[i];
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
                               uint32_t normalsBufferBinding) {
    trianglesBuffer.createShaderStorageBuffer(triangles, trianglesBufferBinding);
    verticesBuffer.createShaderStorageBuffer(vertices, verticesBufferBinding);
    if(!normals.empty())
    normalsBuffer.createShaderStorageBuffer(normals, normalsBufferBinding);
}

void TriangleMesh::clear(){
    triangles.clear();
    vertices.clear();
    normals.clear();
}

uint32_t TriangleMesh::getBeginOfMeshTriangles() const{
    return beginOfMeshTriangles;
}
uint32_t TriangleMesh::getEndOfMeshTriangles() const{
    return endOfMeshTriangles;
}