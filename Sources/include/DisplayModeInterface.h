#pragma once

#include "GLSLShader.h"
#include "Scene.h"

//shaders minimum one but probably more

//Meshes

class DisplayModeInterface {
public:

    virtual void loadShaders() = 0;

    virtual void setUp() = 0;

//    virtual void frameUpdate() = 0;

    virtual void draw(const Scene& scene) = 0;

    virtual ~DisplayModeInterface() = default;

private:
};