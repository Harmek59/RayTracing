#include "OpenGLHandler.h"
#include <stdexcept>


OpenGLHandler::OpenGLHandler(unsigned majorVersjon, unsigned minorVersion, 
    unsigned screenWidth, unsigned screenHeight, const std::string& title){
    initGLFW(majorVersjon, minorVersion);
    createWindow(screenWidth, screenHeight, title);
    initGLAD();
    glEnable(GL_DEPTH_TEST);
}

OpenGLHandler::~OpenGLHandler(){
    glfwTerminate();
}

GLFWwindow* OpenGLHandler::getWindow(){
    return window;
}

void OpenGLHandler::initGLFW(unsigned majorVersjon, unsigned minorVersion){
    glfwInit();
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, majorVersjon);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minorVersion);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

void OpenGLHandler::createWindow(unsigned screenWidth, unsigned screenHeight, const std::string& title){
    window = glfwCreateWindow(screenWidth, screenHeight, title.c_str(), NULL, NULL);
    if (window == NULL)
    {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }
    glfwMakeContextCurrent(window);
}

void OpenGLHandler::initGLAD(){
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        throw std::runtime_error("Failed to initialize GLAD");
}