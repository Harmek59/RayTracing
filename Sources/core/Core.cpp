#include "Core.h"
#include "GLFWCallbacks.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

void Core::setUp() {
    oglHandler = std::make_unique<OpenGLHandler>(oglVersionMajor, oglVersionMinor, windowWidth, windowHeight, title);

    glfwSetFramebufferSizeCallback(oglHandler->getWindow(), GLFWCallbacks::framebuffer_size_callback);
    glfwSetCursorPosCallback(oglHandler->getWindow(), GLFWCallbacks::mouse_callback);

    mouseCaptured = false;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void) io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(Core::getWindow(), true);
    ImGui_ImplOpenGL3_Init("#version 430");
}

bool Core::checkIfMainLoopShouldBreak() {
    return glfwWindowShouldClose(getWindow());
}

void Core::swapBuffers() {
    glfwSwapBuffers(getWindow());
}

void Core::pollEvents() {
    glfwPollEvents();
}

GLFWwindow *Core::getWindow() {
    return oglHandler->getWindow();
}

Camera &Core::getCamera() {
    return camera;
}

void GLAPIENTRY debugMessageCallback(GLenum source,
                                     GLenum type,
                                     GLuint id,
                                     GLenum severity,
                                     GLsizei length,
                                     const GLchar *message,
                                     const void *userParam) {
    if (type != GL_DEBUG_TYPE_OTHER) {
        fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
                (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : std::to_string(int(type)).c_str()),
                type, severity, message);
    }
}

void Core::enableDebugMessages() {
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(debugMessageCallback, 0);
}

void Core::disableDebugMessages() {

}

void Core::enableFaceCulling() {
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glFrontFace(GL_CCW);
}

void Core::enableDepthTest() {
    glEnable(GL_DEPTH_TEST);
}

void Core::processInput() {
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
            releaseMouse();
        } else {
            captureMouse();
        }


    }
}

void Core::preFrameLogic() {
    float currFrameTime = static_cast<float>(glfwGetTime());
    deltaTime = currFrameTime - prevFrameTime;
    prevFrameTime = currFrameTime;

    processInput();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void Core::postFrameLogic() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    Core::swapBuffers();
    Core::pollEvents();
}

float Core::getDeltaTime() {
    return deltaTime;
}

uint32_t Core::getWindowHeight() {
    return windowHeight;
}

uint32_t Core::getWindowWidth() {
    return windowWidth;
}

bool Core::isMouseCaptured() {
    return mouseCaptured;
}

void Core::enableVsync() {
    glfwSwapInterval(1);
}

void Core::disableVsync() {
    glfwSwapInterval(0);
}

void Core::captureMouse() {
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
    glfwSetInputMode(oglHandler->getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    mouseCaptured = true;
}

void Core::releaseMouse() {
    ImGui::GetIO().ConfigFlags ^= ImGuiConfigFlags_NoMouse;
    glfwSetInputMode(oglHandler->getWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    mouseCaptured = false;
    camera.setFirstMouseMove(true);
}