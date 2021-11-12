//TODO: zmien aby chowanie modelu (hide) działało w taki sposób, aby nie wysyłać tych danych na GPU, a nie tak jak jest teraz wysylam wszystko i w shaderach sprawdzam czy jest hide czy nie

#include "imgui.h"


#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include "BVHTree.h"

#include "Texture2D.h"
#include "Core.h"
#include "DisplayModeNormal.h"
#include "DisplayModes/DisplayModeGrid.h"
#include "DisplayModeDirectionalShadowMap.h"
#include "DisplayModes/DisplayModeAABB.h"
#include "DisplayModePointShadowCubeMap.h"
#include "DisplayModeTree.h"
#include "Scene.h"
#include "Buffer.h"
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
    Core::setUp();
    Core::enableDepthTest();
    Core::disableVsync();
//    Core::enableVsync();
    Core::captureMouse();

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

    Core::getCamera().setMovementSpeed(40.);
    Core::getCamera().setRotation(glm::radians(180.f), 0.f, 0.f);

    Material materials;
    materials.loadFromFile("../Resources/Materials.txt");
    Material::bindBuffers(4);

    std::map<std::string, Scene> scenes;
    scenes.emplace("1_BasicScene", Scene("../Resources/Scenes/Scene1.txt"));
    scenes.emplace("2_Ball", Scene("../Resources/Scenes/Scene2.txt"));
    scenes.emplace("2_Cats", Scene("../Resources/Scenes/Scene3.txt"));
    scenes.emplace("4_Tree", Scene("../Resources/Scenes/Scene4.txt"));
    scenes.emplace("4_Mirrors", Scene("../Resources/Scenes/Mirror.txt"));
//    scenes.emplace("5_5kk", Scene("../Resources/Scenes/Scene5.txt"));


    Scene *currScene = &scenes.at("1_BasicScene");

    DisplayModeNormal displayModeNormal;
    DisplayModeGrid displayModeRasterizationWithGrids;
    DisplayModeDirectionalShadowMap displayModeDirectionalShadowMap;
    DisplayModePointShadowCubeMap displayModePointShadowCubeMap;
    DisplayModeTree displayModeTree;
    DisplayModeAABB displayModeAabb;

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);


//    glDisable(GL_DEBUG_OUTPUT);

    std::string GPUName = reinterpret_cast<const char *>(glGetString(GL_RENDERER));

    std::map<std::string, DisplayModeInterface *> displayModes;
    displayModes["1_normal"] = &displayModeNormal;
    displayModes["3_rasterWithGrid"] = &displayModeRasterizationWithGrids;
    displayModes["5_directionalShadowMap"] = &displayModeDirectionalShadowMap;
    displayModes["6_pointShadowCubeMap"] = &displayModePointShadowCubeMap;
    displayModes["4_Tree"] = &displayModeTree;
    displayModes["2_AABB"] = &displayModeAabb;

    DisplayModeInterface *displayMode = &displayModeNormal;

    GlobalSettings globalSettings;
    globalSettings.recursionDepth = 3;
    globalSettings.numberOfLights = 0;

    glm::vec3 lightPos = Core::getCamera().getPosition();
    lightPos.y = 300.f;
    lightPos = glm::vec3(0.0, 300., 0.0);
    glm::mat4 lightProjection, lightView;
    glm::mat4 lightSpaceMatrix;
    float near_plane = 1.0f, far_plane = 500.f;
    lightProjection = glm::ortho(-300.0f, 300.0f, -300.0f, 300.0f, near_plane, far_plane);
    lightView = glm::lookAt(lightPos, glm::vec3(lightPos.x - 100, -1., lightPos.z - 300), glm::vec3(0.0, 1.0, 0.0));
    lightSpaceMatrix = lightProjection * lightView;

    globalSettings.lightSpaceMatrix = lightSpaceMatrix; //for now fixed TODO make it dynamic


    globalSettings.inverseViewMatrix = glm::inverse(
            Core::getCamera().getViewMatrix());  //we need to invert view matrix, because everything inside view matrix is inverted (position -> -positon itp.)
    globalSettings.viewMatrix = Core::getCamera().getViewMatrix();
    globalSettings.cameraPosition = Core::getCamera().getPosition();

    Buffer globSettBuffer(sizeof(GlobalSettings), GL_DYNAMIC_DRAW);

    auto updateGlobSettBuffer = [&globalSettings, &globSettBuffer]() {
        globSettBuffer.bind(GL_UNIFORM_BUFFER);
        GlobalSettings *block = (GlobalSettings *) globSettBuffer.mapBuffer(GL_UNIFORM_BUFFER, GL_READ_WRITE |
                                                                                               GL_MAP_INVALIDATE_BUFFER_BIT);
        if (!block) {
            throw std::runtime_error("Scene::bindSceneDataBufferBase");
        }

        *block = globalSettings;

        globSettBuffer.unMap(GL_UNIFORM_BUFFER);
        globSettBuffer.unBind();
    };

    updateGlobSettBuffer();
    globSettBuffer.bindBufferBase(GL_UNIFORM_BUFFER, 6);

    while (!Core::checkIfMainLoopShouldBreak()) {
        Core::preFrameLogic();


        Camera &camera = Core::getCamera();

        globalSettings.inverseViewMatrix = glm::inverse(
                Core::getCamera().getViewMatrix()); //inverse: see above
        globalSettings.viewMatrix = Core::getCamera().getViewMatrix();
        globalSettings.cameraPosition = Core::getCamera().getPosition();

        updateGlobSettBuffer();

        gui.drawMainInterface(displayModes, &displayMode, globalSettings);

        gui.drawSceneInterface(scenes, &currScene);

        auto deltaTime = Core::getDeltaTime();

        currScene->atFrameBegin(deltaTime);


        glActiveTexture(GL_TEXTURE3);
        cubeMap.bind();

        displayMode->draw(*currScene);


        ImGui::End();
        Core::postFrameLogic();
    }

    return 0;
}
