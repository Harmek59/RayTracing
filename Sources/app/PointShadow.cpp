#include "PointShadow.h"

PointShadow::PointShadow() {
    setUp();
}

void PointShadow::setUp() {
    loadShaders();
    shadowFrameBuffer.attachTextureToBuffer(shadowMapTexture, GL_DEPTH_ATTACHMENT, {GL_NONE});

}

void PointShadow::loadShaders() {
    pointLightDepthShader = std::make_unique<GLSLShader>(pointLightDepthShaderPath_vert,
                                                         pointLightDepthShaderPath_frag,
                                                         pointLightDepthShaderPath_geom);
}

void PointShadow::generateShadow(const std::vector<Model>& models) {
    glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), (float) SHADOW_WIDTH / (float) SHADOW_HEIGHT, near_plane,
                                            far_plane);
    std::vector<glm::mat4> shadowTransforms;
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

    pointLightDepthShader->setMat4("customViewMatrixProjection", glm::mat4(1.f));
    pointLightDepthShader->setBool("useCustomViewProjectionMatrix", true);

    for(const auto& model: models){
        pointLightDepthShader->setInt("modelDataIndex", model.getModelDataIndex());
        glDrawArrays(GL_TRIANGLES, 0, 3 * model.getNumberOfTriangles());
    }

    shadowFrameBuffer.unBind();

    glViewport(0, 0, 1280, 720);

    glActiveTexture(GL_TEXTURE2);
    shadowMapTexture.bind();
}