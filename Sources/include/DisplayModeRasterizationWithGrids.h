#pragma once

#include "DisplayModeInterface.h"
#include "GLSLShader.h"
#include "CoreEngine.h"

class DisplayModeRasterizationWithGrids : public DisplayModeInterface {
public:

    DisplayModeRasterizationWithGrids() {
        setUp();
    }

    virtual void loadShaders() override {
        rasterShader = std::make_unique<GLSLShader>(rasterVertShaderPath, rasterFragShaderPath);
        gridShader = std::make_unique<GLSLShader>(gridVertShaderPath, gridFragShaderPath, gridGeomShaderPath);
    }

    virtual void setUp() override {
        loadShaders();
    }

    virtual void draw(const Scene& scene) override {
        rasterShader->use();
        rasterShader->setMat4("viewMatrix",
                              CoreEngine::getCamera().getViewMatrix());
        rasterShader->setMat4("projectionMatrix",
                              CoreEngine::getCamera().getProjectionMatrix());
        rasterShader->setVec3("cameraPosition", CoreEngine::getCamera().getPosition());

        glDrawArrays(GL_TRIANGLES, 0, 3 * scene.getNumbersOfTriangles());

        gridShader->use();
        gridShader->setMat4("viewMatrix",
                            CoreEngine::getCamera().getViewMatrix());
        gridShader->setMat4("projectionMatrix",
                            CoreEngine::getCamera().getProjectionMatrix());

        glDrawArrays(GL_POINTS, 0, scene.getNumbersOfCells());
    }

private:
    std::unique_ptr<GLSLShader> rasterShader;
    std::unique_ptr<GLSLShader> gridShader;

    std::string rasterVertShaderPath = "../Resources/shaders/rasterization/rasterization.vert";
    std::string rasterFragShaderPath = "../Resources/shaders/rasterization/rasterization.frag";
    std::string gridVertShaderPath = "../Resources/shaders/rasterization/grid.vert";
    std::string gridGeomShaderPath = "../Resources/shaders/rasterization/grid.geom";
    std::string gridFragShaderPath = "../Resources/shaders/rasterization/grid.frag";

};