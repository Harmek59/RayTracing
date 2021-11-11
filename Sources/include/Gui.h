#pragma once

#include "Core.h"
#include "imgui.h"
#include "glm/gtx/string_cast.hpp"
#include <map>
#include <DisplayModes/DisplayModeInterface.h>


struct GlobalSettings {
    glm::mat4 viewMatrix;
    glm::mat4 inverseViewMatrix;
    glm::mat4 lightSpaceMatrix;
    glm::vec3 cameraPosition;
    float fov = 45.;

    int recursionDepth;
    int numberOfLights;

    float gamma = 2.2f;
    float exposure = 1.f;

    uint32_t imageWidth = 1280;
    uint32_t imageHeight = 720;

    uint32_t useSkyBox = 1;  //TODO I think bool in glsl require 32 bits, but check it
    uint32_t useFloor = 1;
};

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

    void drawMainInterface(const std::map<std::string, DisplayModeInterface *> &displayModes,
                           DisplayModeInterface **displayMode, GlobalSettings &globalSettings) {
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
                        (*displayMode)->reloadShaders();
                    }
                }

                ImGui::TableNextColumn();
                {
                    for (auto &[k, v] : displayModes) {
                        if (ImGui::Button(k.c_str())) {
                            (*displayMode) = v;
                        }
                    }
                }
                ImGui::TableNextColumn();
                {
                    ImGui::SliderInt("lights number", &globalSettings.numberOfLights, 0, 20);
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

    void clean() {
        ImGui::PopStyleVar();
        ImGui::PopStyleColor();
    }

private:
    ImGuiWindowFlags_ flags;
};