#pragma once

#include "DisplayModeInterface.h"

class DisplayModeDirectionalShadowMap : public DisplayModeInterface {
public:
    DisplayModeDirectionalShadowMap() {
        setUp();
    }

    virtual void setUp() override {
        loadShaders();
    }


    virtual void loadShaders() override {
        drawTextureShader = std::make_unique<GLSLShader>(drawTextureShaderPath_vert, drawTextureShaderPath_frag);
    }

    virtual void draw(const Scene& scene) override {
        drawTextureShader->use();
        drawTextureShader->setInt("drawFromFirstBinding", 1);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

private:
    std::string drawTextureShaderPath_vert = "../Resources/shaders/raytracer/draw_texture.vert";
    std::string drawTextureShaderPath_frag = "../Resources/shaders/raytracer/draw_texture.frag";

    std::unique_ptr<GLSLShader> drawTextureShader;
};