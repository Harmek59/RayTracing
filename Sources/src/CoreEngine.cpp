#include "CoreEngine.h"
#include "GLFWCallbacks.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

void CoreEngine::createCoreEngine(){
    oglHandler = std::unique_ptr<OpenGLHandler>(
        new OpenGLHandler(oglVersionMajor, oglVersionMinor, windowWidth, windowHeight, title));

    glfwSetFramebufferSizeCallback(oglHandler->getWindow(), GLFWCallbacks::framebuffer_size_callback);
    glfwSetCursorPosCallback(oglHandler->getWindow(), GLFWCallbacks::mouse_callback);

    if (mouseCaptured)
        glfwSetInputMode(oglHandler->getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void) io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(CoreEngine::getWindow(), true);
    ImGui_ImplOpenGL3_Init("#version 430");
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

void CoreEngine::enableDepthTest(){
    glEnable(GL_DEPTH_TEST);
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
            ImGui::GetIO().ConfigFlags ^= ImGuiConfigFlags_NoMouse;
            glfwSetInputMode(oglHandler->getWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            mouseCaptured = false;
            camera.setFirstMouseMove(true);
        }
        else {
            ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
            glfwSetInputMode(oglHandler->getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            mouseCaptured = true;
        }


    }
}

void CoreEngine::preFrameLogic(){
    deltaTime = frameCounter.update(static_cast<float>(glfwGetTime()));

    processInput();    

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void CoreEngine::postFrameLogic(){
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    CoreEngine::swapBuffers();
    CoreEngine::pollEvents();
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