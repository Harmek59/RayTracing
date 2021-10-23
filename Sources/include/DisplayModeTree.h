#pragma once

#include "DisplayModeInterface.h"
#include "GLSLShader.h"
#include "CoreEngine.h"

class DisplayModeTree : public DisplayModeInterface {
public:

    DisplayModeTree() {
        setUp();
    }

    virtual void loadShaders() override {
        rasterShader = std::make_unique<GLSLShader>(rasterVertShaderPath, rasterFragShaderPath);
        treeShader = std::make_unique<GLSLShader>(treeVertShaderPath, treeFragShaderPath, treeGeomShaderPath);
    }

    virtual void setUp() override {
        loadShaders();
    }

    virtual void draw(const Scene& scene) override {

        ImGui::SliderInt("Tree layer", &treeLayer, -1, 10);

        rasterShader->use();
        rasterShader->setMat4("viewMatrix",
                              CoreEngine::getCamera().getViewMatrix());
        rasterShader->setMat4("projectionMatrix",
                              CoreEngine::getCamera().getProjectionMatrix());
        rasterShader->setVec3("cameraPosition", CoreEngine::getCamera().getPosition());

        glDrawArrays(GL_TRIANGLES, 0, 3 * scene.getNumbersOfTriangles());

        treeShader->use();
        treeShader->setMat4("viewMatrix",
                            CoreEngine::getCamera().getViewMatrix());
        treeShader->setMat4("projectionMatrix",
                            CoreEngine::getCamera().getProjectionMatrix());
        treeShader->setInt("treeLayer", treeLayer);

        glDrawArrays(GL_POINTS, 0, scene.getTree().getNumberOfNodes());
    }

private:
    std::unique_ptr<GLSLShader> rasterShader;
    std::unique_ptr<GLSLShader> treeShader;

    std::string rasterVertShaderPath = "../Resources/shaders/rasterization/rasterization.vert";
    std::string rasterFragShaderPath = "../Resources/shaders/rasterization/rasterization.frag";
    std::string treeVertShaderPath = "../Resources/shaders/tree.vert";
    std::string treeGeomShaderPath = "../Resources/shaders/tree.geom";
    std::string treeFragShaderPath = "../Resources/shaders/tree.frag";

    int treeLayer = -1;
};