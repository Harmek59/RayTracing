#pragma once
#include <string>
#include <memory>

#include "OpenGLHandler.h"
#include "Camera.h"


class Core{
public:
    Core() = delete;

    static void setUp();

    static bool checkIfMainLoopShouldBreak();

    static void swapBuffers();

    static void pollEvents();

    static GLFWwindow* getWindow();

    static Camera& getCamera();

    static void processInput();

    static void enableFaceCulling();

    static void enableDepthTest();

    static void preFrameLogic();

    static void postFrameLogic();

    static float getDeltaTime();

    static void enableVsync();
    static void disableVsync();

    static uint32_t getWindowHeight();
    static uint32_t getWindowWidth();

    static bool isMouseCaptured();
private:
    inline static short oglVersionMajor = 4;
    inline static short oglVersionMinor = 3;
    inline static uint32_t windowWidth = 1600;
    inline static uint32_t windowHeight = 900;
    inline static std::string title = "Ray tracing demo";

    inline static std::unique_ptr<OpenGLHandler> oglHandler;
    inline static Camera camera = Camera(windowWidth, windowHeight, 45.f, 0.1f, 4096.f);

    inline static float deltaTime = 0.0;
    inline static float prevFrameTime = 0.0f;

    inline static bool mouseCaptured = true;
};