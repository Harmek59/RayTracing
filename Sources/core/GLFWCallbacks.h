#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Camera.h"

class GLFWCallbacks{
public:
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);

    static void mouse_callback(GLFWwindow* window, double xpos, double ypos);

private:
    static Camera& camera;
};