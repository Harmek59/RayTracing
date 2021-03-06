#pragma once
#include "DisplayModeInterface.h"
#include "GLSLShader.h"

class DisplayModeRasterization : public DisplayModeInterface {
public:
    DisplayModeRasterization() {
        setUp();
    }

    virtual void reloadShaders() override {
        loadShaders();
    }

    virtual void draw(const Scene &scene) override {
        rasterShader->use();

        for (const auto &model: scene.getModels()) {
            rasterShader->setInt("modelDataIndex", model.getModelDataIndex());
            glDrawArrays(GL_TRIANGLES, 0, 3 * model.getNumberOfTriangles());
        }
    }

private:
    virtual void loadShaders() override {
        rasterShader = std::make_unique<GLSLShader>(rasterVertShaderPath, rasterFragShaderPath);
    }

    virtual void setUp() override {
        loadShaders();
    }

    std::unique_ptr<GLSLShader> rasterShader;
    std::string rasterVertShaderPath = "../Sources/shaders/rasterization.vert";
    std::string rasterFragShaderPath = "../Sources/shaders/rasterization.frag";
};