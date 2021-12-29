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
        ImGui::Begin("Scene");
        {
            ImGui::SliderInt("Tree layer", &treeLayer, -1, 10);
            ImGui::Separator();
        }
        ImGui::End();
        DisplayModeRasterization::draw(scene);

        treeShader->use();
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

    std::string treeVertShaderPath = "../Sources/shaders/tree.vert";
    std::string treeGeomShaderPath = "../Sources/shaders/pointToCube.geom";
    std::string treeFragShaderPath = "../Sources/shaders/onlyColor.frag";

    int treeLayer = -1;
};