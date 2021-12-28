#pragma once

#include "FrameBuffer.h"
#include "TextureCubeMap.h"
#include "GLSLShader.h"
#include "Model.h"

#include <memory>

class PointShadow {
public:
    PointShadow();

    void setUp();

    void loadShaders();

    void generateShadow(const std::vector<Model>& models);

    inline auto& getLightPositionRef(){
        return lightPos;
    }

    inline float &getLightRangeRef(){
        return far_plane;
    }

private:
    const uint32_t SHADOW_WIDTH = 512, SHADOW_HEIGHT = 512;
    float near_plane = 0.f;
    float far_plane = 150.0f;
    glm::vec3 lightPos = glm::vec3(0.0, 0.0, -70.0);


    std::string pointLightDepthShaderPath_vert = "../Sources/shaders/rasterization.vert";
    std::string pointLightDepthShaderPath_frag = "../Sources/shaders/simpleDepthShader.frag";
    std::string pointLightDepthShaderPath_geom = "../Sources/shaders/simpleDepthShader.geom";
    FrameBuffer shadowFrameBuffer;

    TextureCubeMap shadowMapTexture = TextureCubeMap(SHADOW_WIDTH, SHADOW_HEIGHT, GL_DEPTH_COMPONENT,
                                                     GL_DEPTH_COMPONENT,
                                                     GL_FLOAT);

    std::unique_ptr<GLSLShader> pointLightDepthShader;
};