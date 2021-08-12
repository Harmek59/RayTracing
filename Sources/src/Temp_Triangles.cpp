#include "Temp_Triangles.h"
#include <exception>

glm::vec3 vertices[] = {
    // Back face
    glm::vec3(-0.5f, -0.5f, -0.5f),
    glm::vec3( 0.5f,  0.5f, -0.5f),
    glm::vec3( 0.5f, -0.5f, -0.5f),      
    glm::vec3( 0.5f,  0.5f, -0.5f),
    glm::vec3(-0.5f, -0.5f, -0.5f),
    glm::vec3(-0.5f,  0.5f, -0.5f),

    glm::vec3(-0.5f, -0.5f,  0.5f),
    glm::vec3( 0.5f, -0.5f,  0.5f),
    glm::vec3( 0.5f,  0.5f,  0.5f),
    glm::vec3( 0.5f,  0.5f,  0.5f),
    glm::vec3(-0.5f,  0.5f,  0.5f),
    glm::vec3(-0.5f, -0.5f,  0.5f),

    glm::vec3(-0.5f,  0.5f,  0.5f),
    glm::vec3(-0.5f,  0.5f, -0.5f),
    glm::vec3(-0.5f, -0.5f, -0.5f),
    glm::vec3(-0.5f, -0.5f, -0.5f),
    glm::vec3(-0.5f, -0.5f,  0.5f),
    glm::vec3(-0.5f,  0.5f,  0.5f),

    glm::vec3( 0.5f,  0.5f,  0.5f),
    glm::vec3( 0.5f, -0.5f, -0.5f),
    glm::vec3( 0.5f,  0.5f, -0.5f),   
    glm::vec3( 0.5f, -0.5f, -0.5f),
    glm::vec3( 0.5f,  0.5f,  0.5f),
    glm::vec3( 0.5f, -0.5f,  0.5f), 

    glm::vec3(-0.5f, -0.5f, -0.5f),
    glm::vec3( 0.5f, -0.5f, -0.5f),
    glm::vec3( 0.5f, -0.5f,  0.5f),
    glm::vec3( 0.5f, -0.5f,  0.5f),
    glm::vec3(-0.5f, -0.5f,  0.5f),
    glm::vec3(-0.5f, -0.5f, -0.5f),
  
    glm::vec3(-0.5f,  0.5f, -0.5f),
    glm::vec3( 0.5f,  0.5f,  0.5f),
    glm::vec3( 0.5f,  0.5f, -0.5f),
    glm::vec3( 0.5f,  0.5f,  0.5f),
    glm::vec3(-0.5f,  0.5f, -0.5f),
    glm::vec3(-0.5f,  0.5f,  0.5f)   
};
Temp_Triangles::Temp_Triangles() : trianglesBuffer{ sizeof(vertices) / sizeof(glm::vec3) * sizeof(triangle), GL_DYNAMIC_DRAW }
{
	this->numberOfTriangles = sizeof(vertices) / sizeof(glm::vec3) / 3;
}

void Temp_Triangles::update(double deltaTime)
{

    f += deltaTime * 0.1;

    uint32_t uniformBlockBinding = 2;
    //861
    trianglesBuffer.bind(GL_UNIFORM_BUFFER);
    triangle* t = (triangle*)trianglesBuffer.mapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY | GL_MAP_INVALIDATE_BUFFER_BIT);
    if (!t) {
        throw std::exception("Temp_Triangles error during update function: mapping buffer failed");
    }

    glm::vec3 centerOfFigure(20.0, 0.0, 20.0);

    for (int i = 0; i < numberOfTriangles; i++){

        double multiplication = 20.;
        t[i].vert0 = glm::vec3(multiplication) * vertices[i * 3] + centerOfFigure;
        t[i].vert1 = glm::vec3(multiplication) * vertices[i * 3 + 1] + centerOfFigure;
        t[i].vert2 = glm::vec3(multiplication) * vertices[i * 3 + 2] + centerOfFigure;
        t[i].albedo = glm::vec3(0.2, 0.2, 0.8);
        t[i].specular = glm::vec3(0.05, 0.05, 0.05);

    }

    trianglesBuffer.unMap(GL_UNIFORM_BUFFER);
    trianglesBuffer.unBind();

    trianglesBuffer.bindBufferBase(GL_UNIFORM_BUFFER, uniformBlockBinding);

}