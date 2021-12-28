#pragma once
#include "DisplayModes/DisplayModeInterface.h"

class DisplayModeNormal : public DisplayModeInterface {
public:
    DisplayModeNormal() {
        setUp();
    }

    virtual void reloadShaders() override{
        loadShaders();
    }

    virtual void draw(const Scene& scene) override {
        rayTracingShader->use();
        glBindImageTexture(0, out_color_tex.getTextureID(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

        glDispatchCompute(80, 45, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);


        drawTextureShader->use();
        glActiveTexture(GL_TEXTURE0);
        out_color_tex.bind();

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

private:
    virtual void setUp() override {
        loadShaders();
    }

    virtual void loadShaders() override {
        drawTextureShader = std::make_unique<GLSLShader>(drawTextureShaderPath_vert, drawTextureShaderPath_frag);
        rayTracingShader = std::make_unique<GLSLShader>(rayTracingShaderPath);
    }

    Texture2D out_color_tex{1280, 720, GL_RGBA32F, GL_RGBA, GL_FLOAT};

    std::string drawTextureShaderPath_vert = "../Sources/shaders/draw_texture.vert";
    std::string drawTextureShaderPath_frag = "../Sources/shaders/draw_texture.frag";

    std::string rayTracingShaderPath = "../Sources/shaders/ray_tracing.comp";

    std::unique_ptr<GLSLShader> drawTextureShader;
    std::unique_ptr<GLSLShader> rayTracingShader;
};