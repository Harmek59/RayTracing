#pragma once
#include "DisplayModes/DisplayModeRasterization.h"

class DisplayModeAABB : public DisplayModeRasterization {
public:
    DisplayModeAABB() {
        setUp();
    }

    virtual void reloadShaders() override {
        loadShaders();
        DisplayModeRasterization::reloadShaders();
    }

    virtual void draw(const Scene &scene) override {
        DisplayModeRasterization::draw(scene);

        aabbShader->use();
        for (const auto &model: scene.getModels()) {
            aabbShader->setInt("modelDataIndex", model.getModelDataIndex());
            glDrawArrays(GL_POINTS, 0, model.getNumberOfGrids());
        }
    }

private:
    virtual void loadShaders() override {
        aabbShader = std::make_unique<GLSLShader>(aabbVertShaderPath, aabbFragShaderPath, aabbGeomShaderPath);
    }

    virtual void setUp() override {
        loadShaders();
    }

    std::unique_ptr<GLSLShader> aabbShader;
    std::string aabbVertShaderPath = "../Sources/shaders/aabb.vert";
    std::string aabbGeomShaderPath = "../Sources/shaders/pointToCube.geom";
    std::string aabbFragShaderPath = "../Sources/shaders/onlyColor.frag";
};