#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include "Buffer.h"


class Temp_Spheres{
public:
    struct sphere
    {
        glm::vec3     center;
        float         radius;
        glm::vec4     color;
        glm::vec4     energy;
    };

    Temp_Spheres(uint32_t numberOfSpheres);

    void update(double deltaTime);

private:
    Buffer spheresBuffer;
    uint32_t numberOfSpheres;

    double f = 0.0;
};