#pragma once

#include "GLSLShader.h"
#include "Texture2D.h"
#include "FrameBuffer.h"
#include "Model.h"
#include "Core.h"

class DirectionalShadow {
public:
    DirectionalShadow();

    DirectionalShadow(DirectionalShadow &&) = default;

    void setUp();

    void loadShaders();

    void generateShadow(const std::vector<Model> &models);

    inline const auto &getLightSpaceMatrix() const {
        return lightSpaceMatrix;
    }

private:
    const uint32_t SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;
    glm::mat4 lightSpaceMatrix;

    std::string directionalLightDepthShaderPath_vert = "../Sources/shaders/rasterization.vert";
    std::string directionalLightDepthShaderPath_frag = "../Sources/shaders/directionalLightDepthShader.frag";
    FrameBuffer shadowFrameBuffer;
    Texture2D shadowMapTexture = Texture2D(SHADOW_WIDTH, SHADOW_HEIGHT, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT,
                                           GL_FLOAT);

    std::unique_ptr<GLSLShader> directionalLightDepthShader;

    static constexpr float borderColor[] = {1.0, 1.0, 1.0, 1.0};
};