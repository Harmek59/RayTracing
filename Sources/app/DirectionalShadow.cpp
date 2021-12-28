#include "DirectionalShadow.h"

DirectionalShadow::DirectionalShadow() {
    setUp();
}

void DirectionalShadow::setUp() {
    loadShaders();
    shadowFrameBuffer.attachTextureToBuffer(shadowMapTexture, GL_DEPTH_ATTACHMENT, {GL_NONE});
    shadowMapTexture.bind();
    // set the texture wrapping parameters
    float color[4] = {1.f, 1.f, 1.f, 1.0f};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    shadowMapTexture.unBind();
}

void DirectionalShadow::loadShaders() {
    directionalLightDepthShader = std::make_unique<GLSLShader>(directionalLightDepthShaderPath_vert,
                                                               directionalLightDepthShaderPath_frag);
}

void DirectionalShadow::generateShadow(const std::vector<Model> &models) {
    glm::vec3 lightPos = Core::getCamera().getPosition();
    lightPos = glm::vec3(0.0, 300., 0.0);
    glm::mat4 lightProjection, lightView;

    float near_plane = 1.0f, far_plane = 1000.f;
    lightProjection = glm::ortho(-300.0f, 300.0f, -400.0f, 200.0f, near_plane, far_plane);
    lightView = glm::lookAt(lightPos, glm::vec3(lightPos.x - 100, -1., lightPos.z - 300), glm::vec3(0.0, 1.0, 0.0));
    lightSpaceMatrix = lightProjection * lightView;

    directionalLightDepthShader->use();

    directionalLightDepthShader->setMat4("customViewMatrixProjection", lightSpaceMatrix);
    directionalLightDepthShader->setBool("useCustomViewProjectionMatrix", true);

    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    shadowFrameBuffer.bind();

    glClear(GL_DEPTH_BUFFER_BIT);
    for (const auto &model: models) {
        directionalLightDepthShader->setInt("modelDataIndex", model.getModelDataIndex());
        glDrawArrays(GL_TRIANGLES, 0, 3 * model.getNumberOfTriangles());
    }
    shadowFrameBuffer.unBind();

    // reset viewport
    glViewport(0, 0, 1280, 720);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glActiveTexture(GL_TEXTURE1);
    shadowMapTexture.bind();
}

