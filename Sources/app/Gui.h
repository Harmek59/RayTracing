#pragma once

#include "Core.h"
#include "Material.h"
#include "GlobalSettings.h"
#include <imgui.h>
#include <glm/gtx/string_cast.hpp>
#include <map>
#include <DisplayModes/DisplayModeInterface.h>


class Gui {
public:
    Gui() {
        setUp();
    }

    ~Gui() {
        clean();
    }

    void setUp() {
        flags = ImGuiWindowFlags_(ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
                                  ImGuiWindowFlags_NoSavedSettings);
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(1.f / 3.f, 1.f / 3.f, 1.f / 3.f, 1.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);
    }

    void drawMainInterface(const std::map<std::string, std::shared_ptr<DisplayModeInterface>> &displayModes, std::map<std::string, std::shared_ptr<Scene>> &scenes,
                           std::shared_ptr<DisplayModeInterface> &displayMode, GlobalSettings &globalSettings) {
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(1280, 180));

        static std::string GPUName = reinterpret_cast<const char *>(glGetString(GL_RENDERER));
        static double deltaTimeSum = 0.;
        static int frameCounter = 0;
        static int avgFps = 0;
        static float avgFrameTime = 0.0;
        deltaTimeSum += Core::getDeltaTime();
        frameCounter++;
        if (deltaTimeSum > 0.25) { // update 4 times per second
            avgFrameTime = float(deltaTimeSum / frameCounter);
            avgFps = int(1. / avgFrameTime);
            deltaTimeSum = 0.0;
            frameCounter = 0;
        }

        std::string oglErr = "";
        while (auto err = glGetError())
            oglErr += std::to_string(err) + " ";

        ImGui::Begin("Main", nullptr, flags);
        {
            ImGui::BeginTable("mainInterfaceSplit", 3);
            {
                ImGui::TableNextColumn();
                {
                    ImGui::Text("%s", GPUName.c_str());
                    ImGui::Text("FPS: %d", avgFps);
                    ImGui::Text("Frame time: %f ms", avgFrameTime * 1000);
                    ImGui::Text("%s", std::string(
                            "Camera position: " + glm::to_string(Core::getCamera().getPosition())).c_str());
                    ImGui::Text("OpenGL errors:");
                    ImGui::SameLine();
                    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
                    ImGui::Text("%s", oglErr.c_str());
                    ImGui::PopStyleColor();

                    static bool showDemoWindow = false;
                    ImGui::Checkbox("DemoWindow", &showDemoWindow);
                    if (showDemoWindow) {
                        ImGui::ShowDemoWindow();
                    }
                    if (ImGui::Button("Reload shaders")) {
                        displayMode->reloadShaders();
                    }
                    if(ImGui::Button("Reload scenes")){
                        Material::reload();
                        GridDDA::clear();
                        Model::clear();
                        TriangleMesh::clear();

                        for(auto &[name, scene] : scenes){
                            scene->clear();
                            scene->load();
                        }
                    }
                }

                ImGui::TableNextColumn();
                {
                    for (auto &[k, v] : displayModes) {
                        if (ImGui::Button(k.c_str())) {
                            displayMode = v;
                        }
                    }
                }
                ImGui::TableNextColumn();
                {
                    ImGui::SliderInt("recursion depth", &globalSettings.recursionDepth, 0, 20);
                    ImGui::SliderFloat("gamma:", &globalSettings.gamma, 0.0, 5.0);
                    ImGui::SliderFloat("exposure:", &globalSettings.exposure, 0.0, 10.0);

                    bool skyBox = globalSettings.useSkyBox;
                    ImGui::Checkbox("skyBox", &skyBox);
                    globalSettings.useSkyBox = skyBox;

                    bool floor = globalSettings.useFloor;
                    ImGui::Checkbox("floor", &floor);
                    globalSettings.useFloor = floor;
                }
            }
            ImGui::EndTable();
        }
        ImGui::End();
    }

    void drawSceneInterface(std::map<std::string, std::shared_ptr<Scene>> &scenes,
                            std::shared_ptr<Scene> &currScene) {
        ImGui::SetNextWindowPos(ImVec2(1280, 0));
        ImGui::SetNextWindowSize(ImVec2(329, 900));

        ImGui::Begin("Scene", nullptr, flags);
        {
            ImGui::Text("To apply these 3 changes reload scenes \n(button below information)");
            ImGui::Text("numberOfTrianglesMultiplier:");
            ImGui::SliderFloat("##numberOfTrianglesMultiplier", &DDAGridsCreatorAndSplitter::numberOfTrianglesMultiplier, 0.5f, 100.f);
            ImGui::Text("maxModelsGridResolution:");
            ImGui::SliderInt("##maxModelsGridResolution", &DDAGridsCreatorAndSplitter::maxModelsGridResolution, 64, 8192);
            ImGui::Text("sizeOfSubGrid:");
            ImGui::SliderInt("##sizeOfSubGrid", &DDAGridsCreatorAndSplitter::sizeOfSubGrid, 4, 128);
            ImGui::Separator();
            for (auto&[name, scene] : scenes) {
                if (ImGui::Button(name.c_str())) {
                    currScene = scene;
                }
            }
            ImGui::Separator();

            static int selectedModel = 0;
            if (ImGui::BeginCombo("##combo", std::to_string(
                    selectedModel).c_str())) {
                for (int i = 0; i < int(currScene->getModels().size()); ++i) {
                    bool is_selected = selectedModel == i;
                    if (ImGui::Selectable(std::to_string(i).c_str(), is_selected)) {
                        selectedModel = i;
                    }
                    if (is_selected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }

            if (selectedModel > -1 && selectedModel < int(currScene->getModels().size())) {
                auto &model = currScene->getModels()[selectedModel];
                ImGui::SliderFloat("Position x", &model.getPositionRef().x, -500.f, 500.f);
                ImGui::SliderFloat("Position y", &model.getPositionRef().y, -500.f, 500.f);
                ImGui::SliderFloat("Position z", &model.getPositionRef().z, -500.f, 500.f);
                ImGui::Text(" ");
                ImGui::SliderFloat("Scale x", &model.getScaleRef().x, 0.f, 500.f);
                ImGui::SliderFloat("Scale y", &model.getScaleRef().y, 0.f, 500.f);
                ImGui::SliderFloat("Scale z", &model.getScaleRef().z, 0.f, 500.f);
                ImGui::Text(" ");
                ImGui::SliderFloat("Rotation x", &model.getRotationRef().x, -180.f, 180.f);
                ImGui::SliderFloat("Rotation y", &model.getRotationRef().y, -180.f, 180.f);
                ImGui::SliderFloat("Rotation z", &model.getRotationRef().z, -180.f, 180.f);
                ImGui::Text(" ");
                if (model.getDrawRef()) {
                    if (ImGui::Button("Hide")) {
                        model.getDrawRef() = false;
                    }
                } else {
                    if (ImGui::Button("Show")) {
                        model.getDrawRef() = true;
                    }
                }

            }
            ImGui::Separator();
            ImGui::Text("Point light:");
            ImGui::SliderFloat("Position x##lightPos", &currScene->getPointLightPositionRef().x, -500.f, 500.f);
            ImGui::SliderFloat("Position y##lightPos", &currScene->getPointLightPositionRef().y, -500.f, 500.f);
            ImGui::SliderFloat("Position z##lightPos", &currScene->getPointLightPositionRef().z, -500.f, 500.f);
            ImGui::SliderFloat("Range##lightPos", &currScene->getPointLightRageRef(), 0.f, 1000.f);
            ImGui::Separator();
        }
        ImGui::End();
    }

    void clean() {
        ImGui::PopStyleVar();
        ImGui::PopStyleColor();
    }

private:
    ImGuiWindowFlags_ flags;
};