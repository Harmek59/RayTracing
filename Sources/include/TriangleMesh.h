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
        uint32_t textureCoordsIndex1;
        uint32_t textureCoordsIndex2;
        uint32_t textureCoordsIndex3;
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

    struct TextureCoords {
        glm::vec2 coords;
    };

    TriangleMesh() = default;

    TriangleMesh(std::vector<Vertex> v, std::vector<Triangle> t, std::vector<Normals> n, std::vector<TextureCoords> tc);

    std::pair<glm::vec3, glm::vec3> calculateBoundingBox() const;

    std::pair<glm::vec3, glm::vec3> getBoundingBox() const {
        return {bbBegin, bbEnd};
    }

    auto getNumberOfTriangles() const {
        return std::distance(beginOfMeshTriangles, endOfMeshTriangles);
    }

    void normalize();

    static const auto &getTriangles() {
        return triangles;
    }

    static const auto &getVertices() {
        return vertices;
    }

    static const auto &getNormals() {
        return normals;
    }

    static void
    bindBuffers(uint32_t trianglesBufferBinding, uint32_t verticesBufferBinding, uint32_t normalsBufferBinding,
                uint32_t textureCoordsBufferBinding);

private:
public:
    inline static std::vector<Triangle> triangles;
    inline static std::vector<Vertex> vertices;
    inline static std::vector<Normals> normals;
    inline static std::vector<TextureCoords> textureCoords;

    inline static Buffer trianglesBuffer;
    inline static Buffer verticesBuffer;
    inline static Buffer normalsBuffer;
    inline static Buffer textureCoordsBuffer;

    std::vector<Triangle>::iterator beginOfMeshTriangles;
    std::vector<Triangle>::iterator endOfMeshTriangles;

    std::vector<Vertex>::iterator beginOfMeshVertices;
    std::vector<Vertex>::iterator endOfMeshVertices;

    mutable glm::vec3 bbBegin, bbEnd;


};

