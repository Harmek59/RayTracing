#pragma once

#include "GLSLShader.h"
#include "FrameBuffer.h"

class DirectionalShadow {
public:
    DirectionalShadow() {
        setUp();
    }

    DirectionalShadow(DirectionalShadow&&) = default;

    void setUp() {
        loadShaders();
        shadowFrameBuffer.attachTextureToBuffer(shadowMapTexture, GL_DEPTH_ATTACHMENT, {GL_NONE});

    }

    void loadShaders() {
        directionalLightDepthShader = std::make_unique<GLSLShader>(directionalLightDepthShaderPath_vert,
                                                                   directionalLightDepthShaderPath_frag);
    }

    void generateShadow(int numberOfTriangles) {
        glm::vec3 lightPos = CoreEngine::getCamera().getPosition();
        lightPos.y = 300.f;
        lightPos = glm::vec3(0.0, 300., 0.0); // zmienic tu w shaderze(ray_tracing) i display mode normal
        glm::mat4 lightProjection, lightView;
        glm::mat4 lightSpaceMatrix;
        float near_plane = 1.0f, far_plane = 500.f;
        lightProjection = glm::ortho(-300.0f, 300.0f, -300.0f, 300.0f, near_plane, far_plane);
        lightView = glm::lookAt(lightPos, glm::vec3(lightPos.x - 100, -1., lightPos.z - 300), glm::vec3(0.0, 1.0, 0.0));
        lightSpaceMatrix = lightProjection * lightView;

        directionalLightDepthShader->use();
        directionalLightDepthShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);

        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        shadowFrameBuffer.bind();

        glClear(GL_DEPTH_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, numberOfTriangles * 3);
        shadowFrameBuffer.unBind();

        // reset viewport
        glViewport(0, 0, 1280, 720);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE1);
        shadowMapTexture.bind();
    }

private:
    const uint32_t SHADOW_WIDTH = 8192, SHADOW_HEIGHT = 8192;


    std::string directionalLightDepthShaderPath_vert = "../Resources/shaders/directionalLightDepthShader.vert";
    std::string directionalLightDepthShaderPath_frag = "../Resources/shaders/directionalLightDepthShader.frag";
    FrameBuffer shadowFrameBuffer;
    Texture2D shadowMapTexture = Texture2D(SHADOW_WIDTH, SHADOW_HEIGHT, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT,
                                           GL_FLOAT);

    std::unique_ptr<GLSLShader> directionalLightDepthShader;

    static constexpr float borderColor[] = {1.0, 1.0, 1.0, 1.0};
};