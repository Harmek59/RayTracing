#include "GLFWCallbacks.h"
#include "CoreEngine.h"

void GLFWCallbacks::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    camera.setScreenSize(width, height);
}

void GLFWCallbacks::mouse_callback(GLFWwindow* window, double xPos, double yPos)
{
    if(CoreEngine::isMouseCaptured())
        camera.processMouseMovement(static_cast<float>(xPos), static_cast<float>(yPos));
}

Camera& GLFWCallbacks::camera = CoreEngine::getCamera();
