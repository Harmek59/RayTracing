#pragma once

#include "CoreEngine.h"
#include "imgui.h"
#include "glm/gtx/string_cast.hpp"
#include <map>
#include <DisplayModeInterface.h>

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
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    }

    void drawMainInterface(const std::map<std::string, DisplayModeInterface *> &displayModes,
                           DisplayModeInterface ** displayMode, bool &pause) {
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(1280, 180));

        static std::string GPUName = reinterpret_cast<const char *>(glGetString(GL_RENDERER));
        static double deltaTimeSum = 0.;
        static int frameCounter = 0;
        static int avgFps = 0;
        static float avgFrameTime = 0.0;
        deltaTimeSum += CoreEngine::getDeltaTime();
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


            ImGui::BeginTable("mainInterfaceSplit", 2);
            {
                ImGui::TableNextColumn();
                {
                    ImGui::Text(GPUName.c_str());
                    ImGui::Text("FPS: %d", avgFps);
                    ImGui::Text("Frame time: %f ms", avgFrameTime * 1000);
                    ImGui::Text(std::string(
                            "Camera position: " + glm::to_string(CoreEngine::getCamera().getPosition())).c_str());
                    ImGui::Text("OpenGL errors: %s", oglErr.c_str());
                }

                ImGui::TableNextColumn();
                {
                    for (auto &[k, v] : displayModes) {
                        if (ImGui::Button(k.c_str())) {
                            (*displayMode) = v;
                        }
                    }
                    if (ImGui::Button("Reload shaders")) {
                        (*displayMode)->loadShaders();
                    }
                    if (ImGui::Button("Pause"))
                        pause = !pause;
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