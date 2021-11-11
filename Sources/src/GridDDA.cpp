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
