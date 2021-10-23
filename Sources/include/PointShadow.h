#pragma once

#include "FrameBuffer.h"
#include "TextureCubeMap.h"
#include "GLSLShader.h"

class PointShadow {
public:
    PointShadow() {
        setUp();
    }

    void setUp() {
        loadShaders();
        shadowFrameBuffer.attachTextureToBuffer(shadowMapTexture, GL_DEPTH_ATTACHMENT, {GL_NONE});

    }

    void loadShaders() {
        pointLightDepthShader = std::make_unique<GLSLShader>(pointLightDepthShaderPath_vert,
                                                             pointLightDepthShaderPath_frag,
                                                             pointLightDepthShaderPath_geom);
    }

    void generateShadow(int numberOfTriangles) {
        glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), (float) SHADOW_WIDTH / (float) SHADOW_HEIGHT, near_plane,
                                                far_plane);
        std::vector<glm::mat4> shadowTransforms;
        glm::vec3 lightPos = glm::vec3(-20.0, 20.0, -20.0);
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(1.0f, 0.0f, 0.0f),
                                                            glm::vec3(0.0f, -1.0f, 0.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0f, 0.0f, 0.0f),
                                                            glm::vec3(0.0f, -1.0f, 0.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 1.0f, 0.0f),
                                                            glm::vec3(0.0f, 0.0f, 1.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, -1.0f, 0.0f),
                                                            glm::vec3(0.0f, 0.0f, -1.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, 1.0f),
                                                            glm::vec3(0.0f, -1.0f, 0.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, -1.0f),
                                                            glm::vec3(0.0f, -1.0f, 0.0f)));

        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        shadowFrameBuffer.bind();
        glClear(GL_DEPTH_BUFFER_BIT);

        pointLightDepthShader->use();
        for (unsigned int i = 0; i < 6; ++i) {
            pointLightDepthShader->setMat4("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);

        }
        pointLightDepthShader->setFloat("far_plane", far_plane);
        pointLightDepthShader->setVec3("lightPos", lightPos);
        glDrawArrays(GL_TRIANGLES, 0, numberOfTriangles * 3);
        shadowFrameBuffer.unBind();

        glViewport(0, 0, 1280, 720);

        glActiveTexture(GL_TEXTURE2);
        shadowMapTexture.bind();
    }

private:
    const uint32_t SHADOW_WIDTH = 4096, SHADOW_HEIGHT = 4096;
    float near_plane = 1.0f;
    float far_plane = 500.0f;


    std::string pointLightDepthShaderPath_vert = "../Resources/shaders/simpleDepthShader.vert";
    std::string pointLightDepthShaderPath_frag = "../Resources/shaders/simpleDepthShader.frag";
    std::string pointLightDepthShaderPath_geom = "../Resources/shaders/simpleDepthShader.geom";
    FrameBuffer shadowFrameBuffer;

    TextureCubeMap shadowMapTexture = TextureCubeMap(SHADOW_WIDTH, SHADOW_HEIGHT, GL_DEPTH_COMPONENT,
                                                     GL_DEPTH_COMPONENT,
                                                     GL_FLOAT);

    std::unique_ptr<GLSLShader> pointLightDepthShader;
};