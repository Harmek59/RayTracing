#include "TriangleMesh.h"

#include <set>
#include <algorithm>
#include <iterator>
#include <cassert>

#include <iostream>


TriangleMesh::TriangleMesh(const std::vector<Vertex>& meshVertices)
{
	assert(meshVertices.size() % 3 == 0);

	beginOfMeshTriangles = triangles.end();
	endOfMeshTriangles = triangles.end();

	beginOfMeshVertices = vertices.end();
	endOfMeshVertices = vertices.end();

	setUpTrianglesAndVerticesFromVerticesArray(meshVertices);
}

TriangleMesh::TriangleMesh(std::vector<Vertex> v, std::vector<Triangle> t) {
	auto sizeOfV = v.size();
	auto sizeOfT = t.size();

	uint32_t verticesOffset = vertices.size();
	std::transform(t.begin(), t.end(), std::back_inserter(triangles), [verticesOffset](auto& trg){
	    trg.vertexIndex1 += verticesOffset;
	    trg.vertexIndex2 += verticesOffset;
	    trg.vertexIndex3 += verticesOffset;
	    return std::move(trg);
	});
	std::move(t.begin(), t.end(), std::back_inserter(triangles));
	std::move(v.begin(), v.end(), std::back_inserter(vertices));

	beginOfMeshTriangles = std::prev(triangles.end(), sizeOfT);
	beginOfMeshVertices = std::prev(vertices.end(), sizeOfV);

	endOfMeshTriangles = triangles.end();
	endOfMeshVertices = vertices.end();


	std::cout << "NumberOfTriangle:\t" << std::distance(beginOfMeshTriangles, endOfMeshTriangles) << std::endl;
	std::cout << "NumberOfVertices:\t" << std::distance(beginOfMeshVertices, endOfMeshVertices) << std::endl;
}

void TriangleMesh::setUpTrianglesAndVerticesFromVerticesArray(const std::vector<Vertex>& meshVertices)
{
	auto vertComp = [](const Vertex& vert1, const Vertex& vert2) { return vert1.x < vert2.x; };
	std::set<Vertex, decltype(vertComp)> meshVerticesUnique(vertComp);
	std::vector<uint32_t> meshIndices;

	std::for_each(meshVertices.cbegin(), meshVertices.cend(), [&meshVerticesUnique, &meshIndices](const Vertex& vert) {
		auto insertResult = meshVerticesUnique.insert(vert);
		auto vertPosInSet = std::distance(meshVerticesUnique.begin(), insertResult.first);
		meshIndices.push_back(vertPosInSet);
		});


	auto verticesOffset = vertices.size();
	for (auto i = meshIndices.cbegin(); i != meshIndices.end(); i += 3) {
		Triangle triangle;
		triangle.vertexIndex1 = *i + verticesOffset;
		triangle.vertexIndex2 = *(i + 1) + verticesOffset;
		triangle.vertexIndex3 = *(i + 2) + verticesOffset;
		//triangle.material

		triangles.push_back(std::move(triangle));
	}


	std::move(meshVerticesUnique.begin(), meshVerticesUnique.end(), std::back_inserter(vertices));

	endOfMeshTriangles = triangles.end();
	endOfMeshVertices = vertices.end();
}

std::pair<glm::vec3, glm::vec3>  TriangleMesh::calculateBoundingBox() const {
	float minX, minY, minZ;
	minX = minY = minZ = 99999.; // TODO change to maxFloatVal
	float maxX, maxY, maxZ;
	maxX = maxY = maxZ = -99999.; // TODO change to minFloatVal;

	std::for_each(beginOfMeshVertices, endOfMeshVertices, [&](const auto& vert) {
		///std::cout << "=================\nVertex1: " << vert.x << ", " << vert.y << ", " << vert.z << '\n';
		maxX = std::max(maxX, vert.x);
		minX = std::min(minX, vert.x);
		maxY = std::max(maxY, vert.y);
		minY = std::min(minY, vert.y);
		maxZ = std::max(maxZ, vert.z);
		minZ = std::min(minZ, vert.z);
	});
	std::cout << "BoundingBox:\t" << minX << ", " << minY << ", " << minZ << " | " << maxX << ", " << maxY << ", " << maxZ << std::endl;

	return std::make_pair(glm::vec3{minX, minY, minZ}, glm::vec3{maxX, maxY, maxZ});
}

void TriangleMesh::bindBuffers(uint32_t trianglesBufferBinding, uint32_t verticesBufferBinding) {
    auto bindBuffer = [](Buffer& buff, auto& data, uint32_t bindingBlock) {
        using Type = std::decay_t<decltype(data)>::value_type;
        buff = Buffer(data.size() * sizeof(Type), GL_DYNAMIC_DRAW);
        buff.bind(GL_SHADER_STORAGE_BUFFER);
        Type* dataPtr = (Type*)buff.mapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY | GL_MAP_INVALIDATE_BUFFER_BIT);
        if (!dataPtr) {
            throw std::exception("Temp_Spheres error during update function: mapping buffer failed");
        }
        std::memcpy(dataPtr, data.data(), data.size() * sizeof(Type));

        buff.unMap(GL_SHADER_STORAGE_BUFFER);
        buff.unBind();

        buff.bindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingBlock);
    };
    std::cout<<"\nBinding buffers: trianglesArray: "<<triangles.size()<<", verticesArray: "<<vertices.size()<<std::endl;
    bindBuffer(trianglesBuffer, triangles, trianglesBufferBinding);
    bindBuffer(verticesBuffer, vertices, verticesBufferBinding);

}