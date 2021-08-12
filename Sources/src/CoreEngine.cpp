#include "CoreEngine.h"
#include "GLFWCallbacks.h"

void CoreEngine::createCoreEngine(){
    oglHandler = std::unique_ptr<OpenGLHandler>(
        new OpenGLHandler(oglVersionMajor, oglVersionMinor, windowWidth, windowHeight, title));

    glfwSetFramebufferSizeCallback(oglHandler->getWindow(), GLFWCallbacks::framebuffer_size_callback);
    glfwSetCursorPosCallback(oglHandler->getWindow(), GLFWCallbacks::mouse_callback);

    if (mouseCaptured)
        glfwSetInputMode(oglHandler->getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

bool CoreEngine::checkIfMainLoopShouldBreak(){
    return glfwWindowShouldClose(getWindow());
}

void CoreEngine::swapBuffers(){
   glfwSwapBuffers(getWindow());
}

void CoreEngine::pollEvents(){
   glfwPollEvents();
}

GLFWwindow* CoreEngine::getWindow(){
    return oglHandler->getWindow();
}

Camera& CoreEngine::getCamera(){
    return camera;
}

void CoreEngine::enableFaceCulling(){
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glFrontFace(GL_CCW); 
}

void CoreEngine::processInput()
{
    GLFWwindow *window = getWindow();

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.move(Camera::Directions::FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.move(Camera::Directions::BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.move(Camera::Directions::LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.move(Camera::Directions::RIGHT, deltaTime);

    static bool P_press = false;
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
        P_press = true;
    }
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_RELEASE && P_press) {
        P_press = false;
        if (mouseCaptured) {
            glfwSetInputMode(oglHandler->getWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            mouseCaptured = false;
            camera.firstMouseMove = true;
        }
        else {
            glfwSetInputMode(oglHandler->getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            mouseCaptured = true;
        }


    }
}

void CoreEngine::preFrameLogic(){
    deltaTime = frameCounter.update(static_cast<float>(glfwGetTime()));
 
    processInput();    

    glClearColor(0.4f, 0.4f, 0.9f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void CoreEngine::postFrameLogic(){
    CoreEngine::swapBuffers();
    CoreEngine::pollEvents();
}

void CoreEngine::printFrameLogs(){
    std::cout<<"\n============================================"<<std::endl;
    std::cout<<"FPS: "<<frameCounter.getFPS()<<std::endl;
    std::cout<<"Camera position: "<<getCamera().getPosition().x<<" "<<getCamera().getPosition().y<<" "<<getCamera().getPosition().z<<std::endl;
    std::cout<<"OpenGL errors: "<<glGetError()<<std::endl;
    std::cout<<"YAW: "<<getCamera().yaw<<"; PITCH: "<<getCamera().pitch<<"; ROLL: "<<getCamera().roll<<std::endl;
}

float CoreEngine::getDeltaTime(){
    return deltaTime;
}

uint32_t CoreEngine::getWindowHeight(){
    return windowHeight;
}
uint32_t CoreEngine::getWindowWidth(){
    return windowWidth;
}