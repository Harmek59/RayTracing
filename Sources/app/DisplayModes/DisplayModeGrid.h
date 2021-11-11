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

        //TODO fix bug: szkieletowi wysteje kilka celli z dloni
        gridShader->use();

        gridShader->setMat4("projectionMatrix",
                            Core::getCamera().getProjectionMatrix());

        for (const auto &model: scene.getModels()) {
            gridShader->setInt("modelDataIndex", model.modelDataIndex);
            auto beginEndGridsIdx = model.getGridsBeginEndIndexes();
            for (auto gridIdx = beginEndGridsIdx.first; gridIdx < beginEndGridsIdx.second; gridIdx++) {
                auto gridsNumber =
                        GridDDA::gridDataArray[gridIdx].cellsEndIndex - GridDDA::gridDataArray[gridIdx].cellsBeginIndex;
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
    std::string gridVertShaderPath = "../Resources/shaders/rasterization/grid.vert";
    std::string gridGeomShaderPath = "../Resources/shaders/rasterization/pointToCube.geom";
    std::string gridFragShaderPath = "../Resources/shaders/rasterization/onlyColor.frag";
};