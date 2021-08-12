#pragma once
#include <vector>
#include <array>
#include <glm/glm.hpp>

#include "Buffer.h"

class TriangleMesh
{
public:
	struct Triangle {
		uint32_t vertexIndex1;
		uint32_t vertexIndex2;
		uint32_t vertexIndex3;

		uint32_t material;
	};

	struct Vertex {
		float x;
		float y;
		float z;
		uint32_t : 32; //offset
	};

	TriangleMesh(const std::vector<Vertex>& meshVertices);

	TriangleMesh(std::vector<Vertex> v, std::vector<Triangle> t);

	std::pair<glm::vec3, glm::vec3> calculateBoundingBox() const;

	int getNumberOfTriangles() const {
		return std::distance(beginOfMeshTriangles, endOfMeshTriangles);
	}

	static const auto& getTriangles() {
		return triangles;
	}
	static const auto& getVertices() {
		return vertices;
	}

	static void bindBuffers(uint32_t trianglesBufferBinding, uint32_t verticesBufferBinding);


private:
	void setUpTrianglesAndVerticesFromVerticesArray(const std::vector<Vertex>& meshVertices);

private:
public:
	inline static std::vector<Triangle> triangles;
	inline static std::vector<Vertex> vertices;
	inline static Buffer trianglesBuffer;
	inline static Buffer verticesBuffer;

	std::vector<Triangle>::iterator beginOfMeshTriangles;
	std::vector<Triangle>::iterator endOfMeshTriangles;

	std::vector<Vertex>::iterator beginOfMeshVertices;
	std::vector<Vertex>::iterator endOfMeshVertices;

};

