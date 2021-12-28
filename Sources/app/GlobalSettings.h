#pragma once
#include <glm/glm.hpp>

struct GlobalSettings {
    glm::mat4 viewMatrix;
    glm::mat4 inverseViewMatrix;
    glm::mat4 projectionMatrix;
    glm::mat4 lightSpaceMatrix;
    glm::vec3 cameraPosition;
    float fov = 45.;
    glm::vec3 pointLightPosition;
    float pointLightRange;

    int recursionDepth;

    float gamma = 2.2f;
    float exposure = 1.f;

    uint32_t imageWidth = 1280;
    uint32_t imageHeight = 720;

    uint32_t useSkyBox = 1;
    uint32_t useFloor = 1;
};
