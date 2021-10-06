#include "imgui.h"


#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include "GLSLShader.h"
#include "Texture2D.h"
#include "CoreEngine.h"
#include "Buffer.h"
#include "Temp_Lights.h"
#include "Model.h"
#include "DisplayModeNormal.h"
#include "DisplayModePerformanceMeasure.h"
#include "DisplayModeRasterizationWithGrids.h"
#include "DisplayModeShadowCubeMap.h"
#include "DisplayModeDirectionalShadowMap.h"
#include "Scene.h"

#include <vector>
#include <string>
#include <map>

void GLAPIENTRY
MessageCallback(GLenum source,
                GLenum type,
                GLuint id,
                GLenum severity,
                GLsizei length,
                const GLchar *message,
                const void *userParam) {
    if (type != GL_DEBUG_TYPE_OTHER) {
        fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
                (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : std::to_string(int(type)).c_str()),
                type, severity, message);
    }
}

int main() {
    CoreEngine::createCoreEngine();


    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);

    CoreEngine::getCamera().setMovementSpeed(40.);
    CoreEngine::getCamera().setRotation(glm::radians(180.f), 0.f, 0.f);

    std::map<std::string, Scene> scenes;
    scenes.emplace("1_BasicScene", Scene("../Resources/Scenes/Scene1.txt"));
    scenes.emplace("2_Ball", Scene("../Resources/Scenes/Scene2.txt"));

    Scene *currScene = &scenes.begin()->second;

    DisplayModeNormal displayModeNormal;
    DisplayModePerformanceMeasure displayModePerformanceMeasure;
    DisplayModeRasterizationWithGrids displayModeRasterizationWithGrids;
    DisplayModeShadowCubeMap displayModeShadowCubeMap;
    DisplayModeDirectionalShadowMap displayModeDirectionalShadowMap;

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);


    glDisable(GL_DEBUG_OUTPUT);

    std::string GPUName = reinterpret_cast<const char *>(glGetString(GL_RENDERER));

    std::map<std::string, DisplayModeInterface *> displayModes;
    displayModes["normal"] = &displayModeNormal;
    displayModes["perfMeasure"] = &displayModePerformanceMeasure;
    displayModes["rasterWithGrid"] = &displayModeRasterizationWithGrids;
    displayModes["directionalShadowMap"] = &displayModeDirectionalShadowMap;
//    displayModes["ShadowCubeMap"] = &displayModeShadowCubeMap;

    DisplayModeInterface *displayMode = &displayModeNormal;

    double deltaTimeSum = 0.;
    int frameCounter = 0;
    int avgFps = 0;
    float avgFrameTime = 0.0;

    bool pause = false;
    while (!CoreEngine::checkIfMainLoopShouldBreak()) {
        CoreEngine::preFrameLogic();

        Camera &camera = CoreEngine::getCamera();

        ImGui::Text(GPUName.c_str());
        deltaTimeSum += CoreEngine::getDeltaTime();
        frameCounter++;
        if(deltaTimeSum > 0.25){    //update 4 times per second
            avgFrameTime = float(deltaTimeSum / frameCounter);
            avgFps = int(1. / avgFrameTime);
            deltaTimeSum = 0.0;
            frameCounter = 0;
        }
        ImGui::Text("FPS: %d", avgFps);
        ImGui::Text("Frame time: %f", avgFrameTime * 1000);
        ImGui::Text("Camera position: %f, %f, %f", camera.getPosition().x, camera.getPosition().y,
                    camera.getPosition().z);


        std::string oglErr = "";
        while (auto err = glGetError())
            oglErr += std::to_string(err);
        ImGui::Text("OpenGL errors: %s", oglErr.c_str());
        ImGui::Text("===================");
        for (auto &[k, v] : displayModes) {
            if (ImGui::Button(k.c_str())) {
                displayMode = v;
            }
        }
        if (ImGui::Button("Reload shaders")) {
            displayMode->loadShaders();
        }
        if (ImGui::Button("Pause"))
            pause = !pause;


        auto deltaTime = CoreEngine::getDeltaTime();
        if (pause)
            deltaTime = 0.0;

        ImGui::Begin("Scene");
        for (auto&[name, scene] : scenes) {
            if (ImGui::Button(name.c_str())) {
                currScene = &scene;
            }
        }
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
            ImGui::SliderFloat("Position x:", &model.position.x, -500.f, 500.f);
            ImGui::SliderFloat("Position y:", &model.position.y, -500.f, 500.f);
            ImGui::SliderFloat("Position z:", &model.position.z, -500.f, 500.f);
            ImGui::Text("");
            ImGui::SliderFloat("Scale x:", &model.scale.x, 0.f, 500.f);
            ImGui::SliderFloat("Scale y:", &model.scale.y, 0.f, 500.f);
            ImGui::SliderFloat("Scale z:", &model.scale.z, 0.f, 500.f);
            ImGui::Text("");
            ImGui::SliderFloat("Rotation x:", &model.rotation.x, -180.f, 180.f);
            ImGui::SliderFloat("Rotation y:", &model.rotation.y, -180.f, 180.f);
            ImGui::SliderFloat("Rotation z:", &model.rotation.z, -180.f, 180.f);
            ImGui::Text("");
            if (model.draw) {
                if (ImGui::Button("Hide")) {
                    model.draw = false;
                }
            } else {
                if (ImGui::Button("Show")) {
                    model.draw = true;
                }
            }

        }
        ImGui::End();

        currScene->atFrameBegin(deltaTime);

        displayMode->draw(*currScene);


        CoreEngine::postFrameLogic();
    }
    return 0;
}
