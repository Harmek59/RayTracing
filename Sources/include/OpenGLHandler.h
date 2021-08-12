#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>

class OpenGLHandler{
public:
    OpenGLHandler(unsigned majorVersjon, unsigned minorVersion, 
        unsigned screenWidth, unsigned screenHeight, const std::string& title);

    ~OpenGLHandler();

    GLFWwindow* getWindow();

    void createWindow(unsigned screenWidth, unsigned screenHeight, const std::string& title);

    void initGLFW(unsigned majorVersjon, unsigned minorVersion);

    void initGLAD();

private:
    GLFWwindow* window = nullptr;
};