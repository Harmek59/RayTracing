#pragma once

#include "DisplayModes/DisplayModeRasterization.h"

#include <imgui.h>

class DisplayModeTree : public DisplayModeRasterization {
public:

    DisplayModeTree() {
        setUp();
    }


    virtual void reloadShaders() override {
        loadShaders();
        DisplayModeRasterization::reloadShaders();
    }


    virtual void draw(const Scene &scene) override {
        ImGui::SliderInt("Tree layer", &treeLayer, -1, 10);

        DisplayModeRasterization::draw(scene);

        treeShader->use();
        treeShader->setMat4("viewMatrix",
                            Core::getCamera().getViewMatrix());
        treeShader->setMat4("projectionMatrix",
                            Core::getCamera().getProjectionMatrix());
        treeShader->setInt("treeLayer", treeLayer);

        glDrawArrays(GL_POINTS, 0, scene.getTree().getNumberOfNodes());
    }

private:

    virtual void loadShaders() override {
        treeShader = std::make_unique<GLSLShader>(treeVertShaderPath, treeFragShaderPath, treeGeomShaderPath);
    }

    virtual void setUp() override {
        loadShaders();
    }

    std::unique_ptr<GLSLShader> treeShader;

    std::string treeVertShaderPath = "../Resources/shaders/tree.vert";
    std::string treeGeomShaderPath = "../Resources/shaders/tree.geom";
    std::string treeFragShaderPath = "../Resources/shaders/tree.frag";

    int treeLayer = -1;
};