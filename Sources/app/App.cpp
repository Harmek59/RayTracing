#include "App.h"

#include "Core.h"
#include "Gui.h"
#include "Material.h"

#include "DisplayModes/DisplayModeNormal.h"
#include "DisplayModes/DisplayModeTree.h"
#include "DisplayModes/DisplayModePointShadowCubeMap.h"
#include "DisplayModes/DisplayModeDirectionalShadowMap.h"
#include "DisplayModes/DisplayModeInterface.h"
#include "DisplayModes/DisplayModeAABB.h"
#include "DisplayModes/DisplayModeRasterization.h"
#include "DisplayModes/DisplayModeGrid.h"


App::App() {
    Core::setUp();
    Core::enableDepthTest();
    Core::disableVsync();
    Core::captureMouse();
//    Core::enableDebugMessages();
//    Core::enableVsync();

    gui = std::make_unique<Gui>();

    skyBox = TextureCubeMap(
            {"../Resources/skybox/right.jpg", "../Resources/skybox/left.jpg", "../Resources/skybox/top.jpg",
             "../Resources/skybox/bottom.jpg", "../Resources/skybox/front.jpg", "../Resources/skybox/back.jpg"});

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    Core::getCamera().setMovementSpeed(40.);
    Core::getCamera().setRotation(glm::radians(180.f), 0.f, 0.f);


    Material::loadFromFile("../Resources/Materials.txt");
    Material::bindBuffers(4);


    scenes.emplace("1_Camels", std::make_shared<Scene>("../Resources/Scenes/Camels.txt"));
    scenes.emplace("2_Sphere", std::make_shared<Scene>("../Resources/Scenes/Sphere.txt"));
    scenes.emplace("3_Mirrors", std::make_shared<Scene>("../Resources/Scenes/Mirror.txt"));
    scenes.emplace("4_Tree", std::make_shared<Scene>("../Resources/Scenes/Tree.txt"));
    scenes.emplace("5_Spiders", std::make_shared<Scene>("../Resources/Scenes/Spiders.txt"));
    currScene = scenes.at("1_Camels");


    displayModes.emplace("1_Normal", std::make_shared<DisplayModeNormal>());
    displayModes.emplace("2_AABB", std::make_shared<DisplayModeAABB>());
    displayModes.emplace("3_Tree", std::make_shared<DisplayModeTree>());
    displayModes.emplace("4_RasterWithGrid", std::make_shared<DisplayModeGrid>());
    displayModes.emplace("5_DirectionalShadowMap", std::make_shared<DisplayModeDirectionalShadowMap>());
    displayModes.emplace("6_PointShadowCubeMap", std::make_shared<DisplayModePointShadowCubeMap>());
    currDisplayMode = displayModes.at("1_Normal");


    globalSettings.recursionDepth = 3;

    globalSettings.inverseViewMatrix = glm::inverse(Core::getCamera().getViewMatrix());
    globalSettings.viewMatrix = Core::getCamera().getViewMatrix();
    globalSettings.cameraPosition = Core::getCamera().getPosition();

    globSettBuffer = Buffer{sizeof(GlobalSettings), GL_DYNAMIC_DRAW};
    globSettBuffer.bindBufferBase(GL_UNIFORM_BUFFER, 6);
}

void App::run() {
    while (!Core::checkIfMainLoopShouldBreak()) {
        Core::preFrameLogic();

        Camera &camera = Core::getCamera();
        globalSettings.inverseViewMatrix = glm::inverse(
                Core::getCamera().getViewMatrix()); //inverse: see above
        globalSettings.viewMatrix = Core::getCamera().getViewMatrix();
        globalSettings.projectionMatrix = Core::getCamera().getProjectionMatrix();
        globalSettings.cameraPosition = Core::getCamera().getPosition();

        globalSettings.lightSpaceMatrix = currScene->getDirectionalLightMatrix();
        globalSettings.pointLightPosition = currScene->getPointLightPositionRef();
        globalSettings.pointLightRange = currScene->getPointLightRageRef();

        updateGlobSettingsBuffer();

        gui->drawMainInterface(displayModes, scenes, currDisplayMode, globalSettings);

        gui->drawSceneInterface(scenes, currScene);

        currScene->atFrameBegin();


        glActiveTexture(GL_TEXTURE3);
        skyBox.bind();

        currDisplayMode->draw(*currScene);

        Core::postFrameLogic();
    }
}


void App::updateGlobSettingsBuffer() {
    globSettBuffer.bind(GL_UNIFORM_BUFFER);
    GlobalSettings *block = (GlobalSettings *) globSettBuffer.mapBuffer(GL_UNIFORM_BUFFER, GL_READ_WRITE |
                                                                                           GL_MAP_INVALIDATE_BUFFER_BIT);
    if (!block) {
        throw std::runtime_error("App::bindSceneDataBufferBase");
    }

    *block = globalSettings;

    globSettBuffer.unMap(GL_UNIFORM_BUFFER);
    globSettBuffer.unBind();
}