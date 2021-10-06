#pragma once

#include <fstream>

#include "Model.h"
#include "Buffer.h"
#include "FrameBuffer.h"
#include "imgui.h"


class Scene {
public:

    Scene(const std::string &path) : simpleDepthShader{"../Resources/shaders/simpleDepthShader.vert",
                                                       "../Resources/shaders/simpleDepthShader.frag",
                                                       "../Resources/shaders/simpleDepthShader.geom"},
                                     directionalLightDepthShader{
                                             "../Resources/shaders/directionalLightDepthShader.vert",
                                             "../Resources/shaders/directionalLightDepthShader.frag"} {
        setUp(path);
    }

    void setUp(const std::string &path) {
        sceneData.beginOfModelsAndGrids = uint32_t(Model::modelDataArray.size());
        std::cout << "Loading scene:\n";
        std::ifstream sceneFile(path);
        if (sceneFile.is_open()) {
            while (true) {
                std::string meshPath;
                sceneFile >> meshPath;

                if (sceneFile.eof()) break; // this is here only in case user will leave extra empty line at the end of file

                if (meshPath[0] == '#') { //comment
                    getline(sceneFile, meshPath); //load all remaining data from that line
                    continue;
                }


                glm::vec3 position;
                sceneFile >> position.x >> position.y >> position.z;

                glm::vec3 scale;
                sceneFile >> scale.x >> scale.y >> scale.z;

                int material;
                sceneFile >> material;


                models.emplace_back(meshPath, position, scale, material);

                if (sceneFile.eof()) break;
            }
        }
        sceneData.endOfModelsAndGrids = uint32_t(Model::modelDataArray.size());

        std::cout << "Scene summarize:\n";
        std::cout << "\tnumber of models: " << sceneData.endOfModelsAndGrids - sceneData.beginOfModelsAndGrids << " ("
                  << sceneData.beginOfModelsAndGrids << " - " << sceneData.endOfModelsAndGrids << ")\n";
        std::cout << "\tnumber of triangles: " << getNumbersOfTriangles() << " ("
                  << Model::modelDataArray[sceneData.beginOfModelsAndGrids].beginEndTriangles.x << " - " <<
                  Model::modelDataArray[sceneData.endOfModelsAndGrids - 1].beginEndTriangles.y << ")\n";


        TriangleMesh::bindBuffers(8, 9, 7, 6);
        GridDDA::bindBuffers(10, 11, 12);
        Model::bindBuffers(13);

        bindSceneDataBufferBase(5);

        glGenFramebuffers(1, &directionalLightDepthMapFBO);
        glGenTextures(1, &depthMapTexture);
        glBindTexture(GL_TEXTURE_2D, depthMapTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT,
                     NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        float borderColor[] = {1.0, 1.0, 1.0, 1.0};
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
        // attach depth texture as FBO's depth buffer
        glBindFramebuffer(GL_FRAMEBUFFER, directionalLightDepthMapFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);


//        glGenFramebuffers(1, &depthMapFBO);
//        glGenTextures(1, &depthCubemap);
//        glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
//        for (unsigned int i = 0; i < 6; ++i)
//            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
//        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
//        // attach depth texture as FBO's depth buffer
//        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
//        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
//        glDrawBuffer(GL_NONE);
//        glReadBuffer(GL_NONE);
//        glBindFramebuffer(GL_FRAMEBUFFER, 0);
//
//        glEnable(GL_DEPTH_TEST);
//        glEnable(GL_CULL_FACE);
    }

    void atFrameBegin(double deltaTime) {
        lights.update(deltaTime);
        updateSceneDataBuffer();
        updateModelData();
        Model::updateBuffers();
        ImGui::SliderInt("lights number", &sceneData.numberOfLights, 0, 20);
        ImGui::SliderInt("recursion depth", &sceneData.recursionDepth, 0, 20);


        glm::vec3 lightPos = CoreEngine::getCamera().getPosition();
        lightPos.y = 300.f;
        lightPos = glm::vec3(0.0, 300., 0.0); // zmienic tu w shaderze(ray_tracing) i display mode normal
        glm::mat4 lightProjection, lightView;
        glm::mat4 lightSpaceMatrix;
        float near_plane = 1.0f, far_plane = 500.f;
        lightProjection = glm::ortho(-300.0f, 300.0f, -300.0f, 300.0f, near_plane, far_plane);
        lightView = glm::lookAt(lightPos, glm::vec3(lightPos.x - 100, -1., lightPos.z - 300), glm::vec3(0.0, 1.0, 0.0));
        lightSpaceMatrix = lightProjection * lightView;

        directionalLightDepthShader.use();
        directionalLightDepthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
//directionalLightDepthShader.setMat4("lightSpaceMatrix", CoreEngine::getCamera().getViewMatrix());

        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, directionalLightDepthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        glActiveTexture(GL_TEXTURE0);
        glDrawArrays(GL_TRIANGLES, 0, getNumbersOfTriangles() * 3);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // reset viewport
        glViewport(0, 0, 1280, 720);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, depthMapTexture);

//        // 0. create depth cubemap transformation matrices
//        // -----------------------------------------------
//        float near_plane = 1.0f;
//        float far_plane = 250.0f;
//        glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), (float)1024 / (float)1024, near_plane, far_plane);
//        std::vector<glm::mat4> shadowTransforms;
//        glm::vec3 lightPos = glm::vec3(-20.0, 20.0, -20.0);
//        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
//        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
//        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
//        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
//        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
//        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
//
//        glViewport(0, 0, 1024, 1024);
//        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
//        glClear(GL_DEPTH_BUFFER_BIT);
//
//        simpleDepthShader.use();
//        for (unsigned int i = 0; i < 6; ++i){
//            simpleDepthShader.setMat4("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
//
//        }
//        simpleDepthShader.setFloat("far_plane", far_plane);
//        simpleDepthShader.setVec3("lightPos", lightPos);
//        glDrawArrays(GL_TRIANGLES, 0, 816);
//        glBindFramebuffer(GL_FRAMEBUFFER, 0);
//        glViewport(0, 0, 1280, 720);
//
////        glBindImageTextures(1, depthCubemap, 0, GL_TRUE, 0, GL_READ_WRITE, GL_R32F);
//        glActiveTexture(GL_TEXTURE1);
//        glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
    }

    auto &getModels() {
        return models;
    }

    void updateModelData() {
        for (auto &model: models) {
            model.updateModelData();
        }
    }

    int getNumbersOfTriangles() const {
        int nmbr = 0;
        for (const auto &m: models) {
            nmbr += m.modelDataArray[m.modelDataIndex].beginEndTriangles.y -
                    m.modelDataArray[m.modelDataIndex].beginEndTriangles.x;
        }
        return nmbr;
    }
    int getNumbersOfCells() const {
        int nmbr = 0;
        for (const auto &m: models) {
            nmbr += m.grid.getNumberOfCells();
        }
        return nmbr;
    }



private:
    struct SceneData {
        int recursionDepth = 3;
        int numberOfLights = 0;
        uint32_t beginOfModelsAndGrids = 0;
        uint32_t endOfModelsAndGrids = 0;

    } sceneData;
    static inline Buffer sceneDataBuffer;


    void bindSceneDataBufferBase(uint32_t bindingPoint) {
        // TODO w tym miejscu w kazda nowa scena bedize niszczyc i tworzy bufor od poczatku (zmienic by towrzyl sie tylko raz)
        sceneDataBuffer = Buffer(sizeof(SceneData), GL_DYNAMIC_DRAW);
        updateSceneDataBuffer();
        sceneDataBuffer.bindBufferBase(GL_UNIFORM_BUFFER, bindingPoint);
    }

    void updateSceneDataBuffer() {
        sceneDataBuffer.bind(GL_UNIFORM_BUFFER);
        SceneData *block = (SceneData *) sceneDataBuffer.mapBuffer(GL_UNIFORM_BUFFER, GL_READ_WRITE |
                                                                                      GL_MAP_INVALIDATE_BUFFER_BIT);
        if (!block) {
            throw std::runtime_error("Scene::bindSceneDataBufferBase");
        }

        *block = sceneData;

        sceneDataBuffer.unMap(GL_UNIFORM_BUFFER);
        sceneDataBuffer.unBind();
    }



    std::vector<Model> models;
    Temp_Lights lights{128};


    const unsigned int SHADOW_WIDTH = 10024, SHADOW_HEIGHT = 10024;
    unsigned int directionalLightDepthMapFBO;
    unsigned int depthMapTexture;
    GLSLShader directionalLightDepthShader;
//    unsigned int depthMapFBO;
//    unsigned int depthCubemap;
    GLSLShader simpleDepthShader;
};