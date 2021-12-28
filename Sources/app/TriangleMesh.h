#pragma once

#include <vector>
#include <array>
#include <glm/glm.hpp>

#include "Buffer.h"

class TriangleMesh {
public:
    struct Triangle {
        uint32_t vertexIndex1;
        uint32_t vertexIndex2;
        uint32_t vertexIndex3;
        uint32_t material;
        uint32_t normalIndex1;
        uint32_t normalIndex2;
        uint32_t normalIndex3;
        uint32_t : 32;
    };

    struct Vertex {
        float x;
        float y;
        float z;
        uint32_t : 32; //offset
    };

    struct Normals {
        glm::vec3 normal;
        uint32_t : 32; //offset
    };

    TriangleMesh() = default;

    TriangleMesh(std::vector<Vertex> v, std::vector<Triangle> t, std::vector<Normals> n);

    std::pair<glm::vec3, glm::vec3> calculateBoundingBox() const;

    inline std::pair<glm::vec3, glm::vec3> getBoundingBox() const {
        return {bbBegin, bbEnd};
    }

    inline auto getNumberOfTriangles() const {
        return endOfMeshTriangles - beginOfMeshTriangles;
    }

    void normalize();

    inline static const auto &getTriangles() {
        return triangles;
    }

    inline static const auto &getVertices() {
        return vertices;
    }

    inline static const auto &getNormals() {
        return normals;
    }

    static void
    bindBuffers(uint32_t trianglesBufferBinding, uint32_t verticesBufferBinding, uint32_t normalsBufferBinding);

    static void clear();

    uint32_t getBeginOfMeshTriangles() const;
    uint32_t getEndOfMeshTriangles() const;


private:
    inline static std::vector<Triangle> triangles;
    inline static std::vector<Vertex> vertices;
    inline static std::vector<Normals> normals;

    inline static Buffer trianglesBuffer;
    inline static Buffer verticesBuffer;
    inline static Buffer normalsBuffer;

    uint32_t beginOfMeshTriangles;
    uint32_t endOfMeshTriangles;

    uint32_t beginOfMeshVertices;
    uint32_t endOfMeshVertices;

    mutable glm::vec3 bbBegin, bbEnd;
};

