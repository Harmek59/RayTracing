#pragma once
#include <string>
#include <memory>
#include <iostream>

#include "OpenGLHandler.h"
#include "Camera.h"
#include "FrameCounter.h"


class CoreEngine{
public:
    static void createCoreEngine();

    static bool checkIfMainLoopShouldBreak();

    static void swapBuffers();

    static void pollEvents();

    static GLFWwindow* getWindow();

    static Camera& getCamera();

    static void processInput();

    static void enableFaceCulling();

    static void preFrameLogic();

    static void postFrameLogic();

    static void printFrameLogs();

    static float getDeltaTime();

    static uint32_t getWindowHeight();
    static uint32_t getWindowWidth();

    static bool isMouseCaptured() {
        return mouseCaptured;
    }

private:
    CoreEngine() = delete;

    inline static short oglVersionMajor = 4;
    inline static short oglVersionMinor = 3;
    inline static uint32_t windowWidth = 1600;
    inline static uint32_t windowHeight = 900;
    inline static std::string title = "Ray tracing";

    inline static float deltaTime = 0.0;

    inline static std::unique_ptr<OpenGLHandler> oglHandler;
    inline static Camera camera = Camera(windowWidth, windowHeight, 45.f, 0.1f, 4096.f);
    
    inline static FrameCounter frameCounter;

    inline static bool mouseCaptured = true;
};