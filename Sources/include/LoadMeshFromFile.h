#pragma once
#include <tiny_obj_loader.h>
#include "TriangleMesh.h"

#include"GridDDA.h"

std::pair<TriangleMesh, GridDDA> loadObjMesh(const std::string& path, glm::vec3 position, glm::vec3 multiplier = glm::vec3(1.0f)) {

    std::cout<<"========================================\n";
    std::cout<<"Loading obj mesh: "<<path<<'\n';
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn, err;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str())) {
		throw std::runtime_error(warn + err);
	}

	std::vector<TriangleMesh::Vertex> vertices;
	std::vector<TriangleMesh::Triangle> triangles;


	for (auto iter = attrib.vertices.begin(); iter < attrib.vertices.end(); iter += 3) {
		TriangleMesh::Vertex vert;
		vert.x = (*iter) * multiplier.x + position.x;
		vert.y = (*std::next(iter)) * multiplier.y + position.y;
		vert.z = (*std::next(iter, 2)) * multiplier.z + position.z;

		vertices.push_back(vert);
	}

	for (const auto& shape : shapes) {
		for (unsigned i = 0; i < shape.mesh.indices.size(); i += 3) {
			TriangleMesh::Triangle triangle;
			triangle.vertexIndex1 = shape.mesh.indices[i].vertex_index;
			triangle.vertexIndex2 = shape.mesh.indices[i + 1].vertex_index;
			triangle.vertexIndex3 = shape.mesh.indices[i + 2].vertex_index;
			triangle.material = 0;

			triangles.push_back(triangle);
		}
	}

	TriangleMesh triangleMesh(std::move(vertices), std::move(triangles));

	GridDDA grid(triangleMesh);

	std::cout<<"========================================\n";

	return std::make_pair(triangleMesh, grid);
}
