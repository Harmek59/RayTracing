#pragma once
#include "DisplayModes/DisplayModeRasterization.h"

class DisplayModeGrid : public DisplayModeRasterization {
public:

    DisplayModeGrid() {
        setUp();
    }

    virtual void reloadShaders() override {
        loadShaders();
        DisplayModeRasterization::reloadShaders();
    }

    virtual void draw(const Scene &scene) override {
        DisplayModeRasterization::draw(scene);

        gridShader->use();

        for (const auto &model: scene.getModels()) {
            gridShader->setInt("modelDataIndex", model.getModelDataIndex());
            auto beginEndGridsIdx = model.getGridsBeginEndIndexes();
            for (auto gridIdx = beginEndGridsIdx.first; gridIdx < beginEndGridsIdx.second; gridIdx++) {
                auto gridsNumber =
                        GridDDA::getGridDataArray()[gridIdx].cellsEndIndex - GridDDA::getGridDataArray()[gridIdx].cellsBeginIndex;
                gridShader->setInt("gridDataIndex", gridIdx);
                glDrawArrays(GL_POINTS, 0, gridsNumber);
            }
        }
    }

private:
    virtual void loadShaders() override {
        gridShader = std::make_unique<GLSLShader>(gridVertShaderPath, gridFragShaderPath, gridGeomShaderPath);
    }

    virtual void setUp() override {
        loadShaders();
    }

    std::unique_ptr<GLSLShader> gridShader;
    std::string gridVertShaderPath = "../Sources/shaders/grid.vert";
    std::string gridGeomShaderPath = "../Sources/shaders/pointToCube.geom";
    std::string gridFragShaderPath = "../Sources/shaders/onlyColor.frag";
};