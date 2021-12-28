#pragma once

#include "Gui.h"
#include "Scene.h"
#include "GlobalSettings.h"
#include "DisplayModes/DisplayModeInterface.h"
#include "Buffer.h"
#include "TextureCubeMap.h"
#include "Texture2D.h"

#include <string>
#include <map>
#include <memory>

class App {
public:
    App();

    void run();

private:
    unsigned int VAO;

    std::unique_ptr<Gui> gui;

    GlobalSettings globalSettings;
    Buffer globSettBuffer;

    void updateGlobSettingsBuffer();

    std::map<std::string, std::shared_ptr<Scene>> scenes;
    std::shared_ptr<Scene> currScene;
    std::map<std::string, std::shared_ptr<DisplayModeInterface>> displayModes;
    std::shared_ptr<DisplayModeInterface> currDisplayMode;

    TextureCubeMap skyBox;
};
