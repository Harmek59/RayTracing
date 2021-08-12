#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include "GLSLShader.h"
#include "Texture2D.h"
#include "Vertices.h"
#include "CoreEngine.h"
#include "Buffer.h"
#include "Temp_Spheres.h"
#include "Temp_Triangles.h"
#include "Temp_Lights.h"
#include "LoadMeshFromFile.h"

#include <iostream>
#include <fstream>
#include <cassert>
#include <cmath>
#include <execution>
#include <functional>
#include <vector>

void GLAPIENTRY
MessageCallback(GLenum source,
                GLenum type,
                GLuint id,
                GLenum severity,
                GLsizei length,
                const GLchar *message,
                const void *userParam) {
    fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
            (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
            type, severity, message);
}

struct uniforms_block {
    glm::mat4 mv_matrix;
    glm::mat4 view_matrix;
    glm::mat4 proj_matrix;
};


int main() {
    CoreEngine::createCoreEngine();
    //CoreEngine::enableFaceCulling(); 

//    glEnable(GL_DEBUG_OUTPUT);
//    glDebugMessageCallback(MessageCallback, 0);

    CoreEngine::getCamera().setMovementSpeed(40.);


    Texture2D strzalka{"../Resources/strzalka.jpg"};

    Buffer uniformBuffer(sizeof(uniforms_block), GL_DYNAMIC_DRAW);

    {
        uniformBuffer.bind(GL_UNIFORM_BUFFER);
        uniforms_block *block = (uniforms_block *) uniformBuffer.mapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY |
                                                                                              GL_MAP_INVALIDATE_BUFFER_BIT);
        if (!block) {
            std::cout << "Cos poszlo nie tak" << std::endl;
            return 0;
        }

        glm::mat4 model_matrix = glm::mat4(1.0f);
        model_matrix = glm::scale(model_matrix, glm::vec3(7.0f));
        block->mv_matrix = CoreEngine::getCamera().getViewMatrix() * model_matrix;
        block->view_matrix = CoreEngine::getCamera().getViewMatrix();
        block->proj_matrix = CoreEngine::getCamera().getProjectionMatrix();

        uniformBuffer.unMap(GL_UNIFORM_BUFFER);
        uniformBuffer.unBind();

        uniformBuffer.bindBufferBase(GL_UNIFORM_BUFFER, 0);

    }


    constexpr uint32_t numberOfSpheres = 128;

    Temp_Spheres spheres{numberOfSpheres};

    Temp_Triangles triangles;

    Temp_Lights lights(128);

    auto[mesh, grid] = loadObjMesh("../Resources/meshes/cat.obj", glm::vec3(-20.f, 20.f, -20.f));

    auto[mesh2, grid2] = loadObjMesh("../Resources/meshes/renifer.obj", glm::vec3(30.f, 20.f, 30.f),
                                     glm::vec3(0.3f, 0.3f, 0.3f));

    auto[mesh3, grid3] = loadObjMesh("../Resources/meshes/tree.obj", glm::vec3(0.f, 80.f, 0.f),
                                     glm::vec3(2.f, 2.f, 2.f));

    auto[mesh4, grid4] = loadObjMesh("../Resources/meshes/girl OBJ.obj", glm::vec3(-30.f, 80.f, 30.f),
                                     glm::vec3(20.f, 20.f, 20.f));

    auto[mesh5, grid5] = loadObjMesh("../Resources/meshes/2AnimeDziwczynki.obj", glm::vec3(30.f, 80.f, -40.f),
                                     glm::vec3(50.f));

//    auto[mesh6, grid6] = loadObjMesh("C:\\Users\\ph025\\Desktop\\figurka.obj", glm::vec3(0.f, 20.f, -30.f),
//                                     glm::vec3(0.1f));


    auto[mesh7, grid7] = loadObjMesh("../Resources/meshes/FinalBaseMesh.obj", glm::vec3(-40.f, 80.f, -30.f),
                                     glm::vec3(2.f, 2.f, 2.f));

    auto[mesh8, grid8] = loadObjMesh("../Resources/meshes/FinalBaseMesh.obj", glm::vec3(-40.f, 85.f, -30.f),
                                     glm::vec3(2.f, 2.f, 2.f));
    auto[mesh9, grid9] = loadObjMesh("../Resources/meshes/FinalBaseMesh.obj", glm::vec3(-40.f, 90.f, -30.f),
                                     glm::vec3(2.f, 2.f, 2.f));
    auto[mesh10, grid10] = loadObjMesh("../Resources/meshes/FinalBaseMesh.obj", glm::vec3(-40.f, 95.f, -30.f),
                                       glm::vec3(2.f, 2.f, 2.f));
    auto[mesh11, grid11] = loadObjMesh("../Resources/meshes/FinalBaseMesh.obj", glm::vec3(-40.f, 100.f, -30.f),
                                       glm::vec3(2.f, 2.f, 2.f));


    TriangleMesh::bindBuffers(8, 9);
    GridDDA::bindBuffers(10, 11, 12);


    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);


    GLSLShader drawTextureShader("../Resources/shaders/raytracer/draw_texture.vert",
                                 "../Resources/shaders/raytracer/draw_texture.frag");

    GLSLShader prepareRaysShader("../Resources/shaders/raytracer/prepare_camera_rays.comp");
    GLSLShader rayTracingShader("../Resources/shaders/raytracer/ray_tracing_v2.comp");

    GLSLShader drawVectorsShader("../Resources/shaders/rasterization/draw_vectors_from_textures.vert",
                                 "../Resources/shaders/rasterization/draw_vectors_from_textures.frag",
                                 "../Resources/shaders/rasterization/draw_vectors_from_textures.geom");


    Texture2D direction_tex{1280, 720, GL_RGBA16F, GL_RGBA, GL_FLOAT};
    Texture2D position_tex{1280, 720, GL_RGBA32F, GL_RGBA, GL_FLOAT};

    Texture2D out_color_tex{1280, 720, GL_RGBA32F, GL_RGBA, GL_FLOAT};

    glDisable(GL_DEBUG_OUTPUT);

    spheres.update(CoreEngine::getDeltaTime());
    triangles.update(CoreEngine::getDeltaTime());
    lights.update(CoreEngine::getDeltaTime());

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void) io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(CoreEngine::getWindow(), true);
    ImGui_ImplOpenGL3_Init("#version 430");

    int spheresNumber = 0;
    int lightsNumber = 0;
    int recursionDepth = 1;
    int numberOfCells = 0;

    bool pause = false;
    while (!CoreEngine::checkIfMainLoopShouldBreak()) {
        CoreEngine::preFrameLogic();

        Camera &camera = CoreEngine::getCamera();


        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Text("FPS: %f", 1. / CoreEngine::getDeltaTime());
        ImGui::Text("Camera position: %f, %f, %f", camera.getPosition().x, camera.getPosition().y,
                    camera.getPosition().z);
        std::string oglErr = "";
        while (auto err = glGetError())
            oglErr += std::to_string(err);
        ImGui::Text("OpenGL errors: %s", oglErr.c_str());
        ImGui::Text("===================");
        if (ImGui::Button("Pause"))
            pause = !pause;
        ImGui::SliderInt("spheres number", &spheresNumber, 0, 200);
        ImGui::SliderInt("lights number", &lightsNumber, 0, 20);
        ImGui::SliderInt("recursion depth", &recursionDepth, 0, 20);
        ImGui::SliderInt("numberOfCells", &numberOfCells, 0, 224);


        auto deltaTime = CoreEngine::getDeltaTime();
        if (pause)
            deltaTime = 0.0;

        spheres.update(deltaTime);
        triangles.update(deltaTime);
        lights.update(deltaTime);


        rayTracingShader.use();
        glActiveTexture(GL_TEXTURE0);
        rayTracingShader.setInt("numberOfCells", numberOfCells);
        rayTracingShader.setInt("recursionDepth", recursionDepth);
        rayTracingShader.setMat4("viewMatrix", glm::inverse(
                CoreEngine::getCamera().getViewMatrix()));  //we need to invert view matrix, because everything inside view matrix is inverted (position -> -positon itp.)
        rayTracingShader.setVec3("cameraPosition", CoreEngine::getCamera().getPosition());

        rayTracingShader.setInt("numberOfSpheres", spheresNumber);
        rayTracingShader.setInt("numberOfLights", lightsNumber);

        glBindImageTexture(0, out_color_tex.getTextureID(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
        glBindImageTexture(1, position_tex.getTextureID(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
        glBindImageTexture(2, direction_tex.getTextureID(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16F);

        glDispatchCompute(80, 45, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);


        drawTextureShader.use();
        glActiveTexture(GL_TEXTURE0);
        out_color_tex.bind();
        //strzalka.bind();

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);


        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        CoreEngine::postFrameLogic();
    }
    return 0;
}
