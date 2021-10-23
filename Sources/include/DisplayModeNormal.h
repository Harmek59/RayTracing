#pragma once

#include "DisplayModeInterface.h"
#include "CoreEngine.h"

class DisplayModeNormal : public DisplayModeInterface {
public:
    DisplayModeNormal() {
        setUp();
    }

    virtual void setUp() override {
        loadShaders();
    }


    virtual void loadShaders() override {
        drawTextureShader = std::make_unique<GLSLShader>(drawTextureShaderPath_vert, drawTextureShaderPath_frag);
        rayTracingShader = std::make_unique<GLSLShader>(rayTracingShaderPath);
    }

    virtual void draw(const Scene& scene) override {
        ImGui::SliderFloat("gamma:", &gamma, 0.0, 5.0);
        ImGui::SliderFloat("exposure:", &exposure, 0.0, 10.0);

        rayTracingShader->use();
        rayTracingShader->setMat4("viewMatrix", glm::inverse(
                CoreEngine::getCamera().getViewMatrix()));  //we need to invert view matrix, because everything inside view matrix is inverted (position -> -positon itp.)
        rayTracingShader->setVec3("cameraPosition", CoreEngine::getCamera().getPosition());

        rayTracingShader->setFloat("gamma", gamma);
        rayTracingShader->setFloat("exposure", exposure);


        glm::vec3 lightPos = CoreEngine::getCamera().getPosition();
        lightPos.y = 300.f;
        lightPos = glm::vec3(0.0, 300., 0.0);
        glm::mat4 lightProjection, lightView;
        glm::mat4 lightSpaceMatrix;
        float near_plane = 1.0f, far_plane = 500.f;
        lightProjection = glm::ortho(-300.0f, 300.0f, -300.0f, 300.0f, near_plane, far_plane);
        lightView = glm::lookAt(lightPos, glm::vec3(lightPos.x - 100, -1., lightPos.z - 300), glm::vec3(0.0, 1.0, 0.0));
        lightSpaceMatrix = lightProjection * lightView;

        rayTracingShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);


        glBindImageTexture(0, out_color_tex.getTextureID(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

        glDispatchCompute(80, 45, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);


        drawTextureShader->use();
        glActiveTexture(GL_TEXTURE0);
        out_color_tex.bind();

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

private:

    Texture2D out_color_tex{1280, 720, GL_RGBA32F, GL_RGBA, GL_FLOAT};

    float gamma = 2.2f;
    float exposure = 0.2f;

    std::string drawTextureShaderPath_vert = "../Resources/shaders/raytracer/draw_texture.vert";
    std::string drawTextureShaderPath_frag = "../Resources/shaders/raytracer/draw_texture.frag";

    std::string rayTracingShaderPath = "../Resources/shaders/raytracer/ray_tracing.comp";

    std::unique_ptr<GLSLShader> drawTextureShader;
    std::unique_ptr<GLSLShader> rayTracingShader;

};