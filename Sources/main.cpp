#include "imgui.h"


#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include "BVHTree.h"

#include "Texture2D.h"
#include "CoreEngine.h"
#include "DisplayModeNormal.h"
#include "DisplayModeRasterizationWithGrids.h"
#include "DisplayModeDirectionalShadowMap.h"
#include "DisplayModePointShadowCubeMap.h"
#include "DisplayModeTree.h"
#include "Scene.h"
#include "Material.h"
#include "TextureCubeMap.h"
#include "Gui.h"

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
    CoreEngine::enableDepthTest();
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse; // capturing mouse, i need to disable mouse in imgui as well

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);

    Gui gui;

    TextureCubeMap cubeMap(
            {"../Resources/skybox/right.jpg", "../Resources/skybox/left.jpg", "../Resources/skybox/top.jpg",
             "../Resources/skybox/bottom.jpg", "../Resources/skybox/front.jpg", "../Resources/skybox/back.jpg"});
//    TextureCubeMap cubeMap(
//            {"../Resources/skybox/SwedishRoyalCastle/posx.jpg", "../Resources/skybox/SwedishRoyalCastle/negx.jpg", "../Resources/skybox/SwedishRoyalCastle/posy.jpg",
//             "../Resources/skybox/SwedishRoyalCastle/negy.jpg", "../Resources/skybox/SwedishRoyalCastle/posz.jpg", "../Resources/skybox/SwedishRoyalCastle/negz.jpg"});
    glActiveTexture(GL_TEXTURE3);
    cubeMap.bind();

    CoreEngine::getCamera().setMovementSpeed(40.);
    CoreEngine::getCamera().setRotation(glm::radians(180.f), 0.f, 0.f);

    Material materials;
    materials.loadFromFile("../Resources/Materials.txt");
    Material::bindBuffers(4);

    std::map<std::string, Scene> scenes;
    scenes.emplace("1_BasicScene", Scene("../Resources/Scenes/Scene1.txt"));
    scenes.emplace("2_Ball", Scene("../Resources/Scenes/Scene2.txt"));
    scenes.emplace("2_Cats", Scene("../Resources/Scenes/Scene3.txt"));
    scenes.emplace("4_Tree", Scene("../Resources/Scenes/Scene4.txt"));
//    scenes.emplace("5_5kk", Scene("../Resources/Scenes/Scene5.txt"));


    Scene *currScene = &scenes.at("1_BasicScene");

    DisplayModeNormal displayModeNormal;
    DisplayModeRasterizationWithGrids displayModeRasterizationWithGrids;
    DisplayModeDirectionalShadowMap displayModeDirectionalShadowMap;
    DisplayModePointShadowCubeMap displayModePointShadowCubeMap;
    DisplayModeTree displayModeTree;

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);


//    glDisable(GL_DEBUG_OUTPUT);

    std::string GPUName = reinterpret_cast<const char *>(glGetString(GL_RENDERER));

    std::map<std::string, DisplayModeInterface *> displayModes;
    displayModes["normal"] = &displayModeNormal;
    displayModes["rasterWithGrid"] = &displayModeRasterizationWithGrids;
    displayModes["directionalShadowMap"] = &displayModeDirectionalShadowMap;
    displayModes["pointShadowCubeMap"] = &displayModePointShadowCubeMap;
    displayModes["Tree"] = &displayModeTree;

    DisplayModeInterface *displayMode = &displayModeNormal;


    bool pause = false;

    bool buildAfterDraw = false;

    while (!CoreEngine::checkIfMainLoopShouldBreak()) {
        CoreEngine::preFrameLogic();
        auto start = std::chrono::high_resolution_clock::now();


        Camera &camera = CoreEngine::getCamera();

        gui.drawMainInterface(displayModes, &displayMode, pause);


        auto deltaTime = CoreEngine::getDeltaTime();
        if (pause)
            deltaTime = 0.0;


        ImGui::SetNextWindowPos(ImVec2(1280, 0));
        ImGui::SetNextWindowSize(ImVec2(329, 900));
//        ImGui::Begin("Scene", nullptr, flags);
        ImGui::Begin("Scene");
        ImGui::Checkbox("Build after draw", &buildAfterDraw);

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
            ImGui::SliderFloat3("Position", &model.position.x, -500.f, 500.f);
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

        currScene->atFrameBegin(deltaTime);


        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);

        glActiveTexture(GL_TEXTURE3);
        cubeMap.bind();

        ImGui::Text("Trace: %f ms przed Draw", duration.count() / 1000.);
        displayMode->draw(*currScene);


        ImGui::End();
        CoreEngine::postFrameLogic();
    }
    return 0;
}
