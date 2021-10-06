//#pragma once
//#include <tiny_obj_loader.h>
//#include "TriangleMesh.h"
//
//#include"GridDDA.h"
//
//std::pair<TriangleMesh, GridDDA> loadObjMesh(const std::string& path, glm::vec3 position, glm::vec3 multiplier = glm::vec3(1.0f)) {
//
//    std::cout<<"========================================\n";
//    std::cout<<"Loading obj mesh: "<<path<<'\n';
//	tinyobj::attrib_t attrib;
//	std::vector<tinyobj::shape_t> shapes;
//	std::vector<tinyobj::material_t> materials;
//	std::string warn, err;
//
//	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str())) {
//		throw std::runtime_error(warn + err);
//	}
//
//	std::cout<<warn+err<<std::endl;
//
//	std::cout<<"Materials data:\n";
//	for(auto& material: materials){
//	    std::cout<<"Name: "<<material.name<<"\nTextures:\n"<<std::endl;
//	    std::cout<<"Ambient: "<<material.ambient_texname<<"\n";
//	    std::cout<<"Diffuse: "<<material.diffuse_texname<<"\n";
//	    std::cout<<"Specular: "<<material.specular<<"\n";
//	    std::cout<<"Bump: "<<material.bump_texname<<"\n";
//	    std::cout<<"Displacement: "<<material.displacement_texname<<"\n";
//	    std::cout<<"Reflection: "<<material.reflection_texname<<"\n";
//	    std::cout<<"PBR textures:\n";
//	    std::cout<<"Roughness: "<<material.roughness_texname<<"\n";
//	    std::cout<<"Metallic: "<<material.metallic_texname<<"\n";
//	    std::cout<<"Sheen: "<<material.sheen_texname<<"\n";
//	    std::cout<<"Emissive: "<<material.emissive_texname<<"\n";
//	    std::cout<<"Normal: "<<material.normal_texname<<"\n";
//	    std::cout<<"Unnknown Parameters:\n";
//	    for(auto& [key, val]: material.unknown_parameter){
//	        std::cout<<key<<": "<<val<<"\n";
//	    }
//	}
//
//	std::vector<TriangleMesh::Vertex> vertices;
//	std::vector<TriangleMesh::Triangle> triangles;
//	std::vector<TriangleMesh::Normals> normals;
//	std::vector<TriangleMesh::TextureCoords> texCoords;
//
//	for (auto iter = attrib.vertices.begin(); iter < attrib.vertices.end(); iter += 3) {
//		TriangleMesh::Vertex vert;
//		vert.x = (*iter) * multiplier.x + position.x;
//		vert.y = (*std::next(iter)) * multiplier.y + position.y;
//		vert.z = (*std::next(iter, 2)) * multiplier.z + position.z;
//
//		vertices.push_back(vert);
//	}
//
//	for (auto iter = attrib.normals.begin(); iter < attrib.normals.end(); iter += 3) {
//	    TriangleMesh::Normals normal;
//	    normal.normal = glm::normalize(glm::vec3(*iter, *std::next(iter), *std::next(iter, 2)));
//
//	    normals.push_back(std::move(normal));
//	}
//
//	for (auto iter = attrib.texcoords.begin(); iter < attrib.texcoords.end(); iter += 2) {
//	    TriangleMesh::TextureCoords coords;
//	    coords.coords = glm::vec2(*iter, - *std::next(iter));       // y must be inverted
//
//	    texCoords.push_back(std::move(coords));
//	}
//
//	for (const auto& shape : shapes) {
//		for (unsigned i = 0; i < shape.mesh.indices.size(); i += 3) {
//
//
//			TriangleMesh::Triangle triangle;
//			triangle.vertexIndex1 = shape.mesh.indices[i].vertex_index;
//			triangle.vertexIndex2 = shape.mesh.indices[i + 1].vertex_index;
//			triangle.vertexIndex3 = shape.mesh.indices[i + 2].vertex_index;
//			if(path.find("sword") != std::string::npos){
//			    triangle.material = 4;//shape.mesh.material_ids[i];
//			}else if(path.find("renifer") != std::string::npos){ //przezrioczyste
//			    triangle.material = 2;
//			}else{
//			    triangle.material = 3;//shape.mesh.material_ids[i];
//			}
//
//
//			triangle.normalIndex1 = shape.mesh.indices[i].normal_index;
//			triangle.normalIndex2 = shape.mesh.indices[i + 1].normal_index;
//			triangle.normalIndex3 = shape.mesh.indices[i + 2].normal_index;
//
//			triangle.textureCoordsIndex1 = shape.mesh.indices[i].texcoord_index;
//			triangle.textureCoordsIndex2 = shape.mesh.indices[i + 1].texcoord_index;
//			triangle.textureCoordsIndex3 = shape.mesh.indices[i + 2].texcoord_index;
//
//			triangles.push_back(triangle);
//		}
//	}
//
//
//	TriangleMesh triangleMesh(std::move(vertices), std::move(triangles), std::move(normals), std::move(texCoords));
//
//	GridDDA grid(triangleMesh);
//
//	std::cout<<"========================================\n";
//
//	return std::make_pair(triangleMesh, grid);
//}
