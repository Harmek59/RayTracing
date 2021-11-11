#pragma once
#include "Scene.h"

class DisplayModeInterface {
public:
    virtual void reloadShaders() = 0;

    virtual void draw(const Scene &scene) = 0;

    virtual ~DisplayModeInterface() = default;

private:
    virtual void loadShaders() = 0;

    virtual void setUp() = 0;
};