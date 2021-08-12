#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include "Buffer.h"


class Temp_Triangles
{
public:
	struct triangle {
        glm::vec3 vert0;
        float offset1;
        glm::vec3 vert1;
        float offset2;
        glm::vec3 vert2;
        float offset3;
        glm::vec3 albedo;
        float offset4;
        glm::vec3 specular;
        float offset5;
	};

    Temp_Triangles();

    void update(double deltaTime);


private:
    Buffer trianglesBuffer;
    uint32_t numberOfTriangles;

    double f = 0.0;
};

