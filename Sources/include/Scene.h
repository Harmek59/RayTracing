#pragma once

#include <fstream>

#include "Model.h"
#include "Buffer.h"
#include "FrameBuffer.h"
#include <imgui.h>
#include "DirectionalSadow.h"
#include "PointShadow.h"
#include "BVHTree.h"


class Scene {
public:

    Scene(const std::string &path) {
        setUp(path);
        bvhTree = BVHTree(models.size() * 5);
    }

    void setUp(const std::string &path) {
        sceneData.beginOfModelsAndGrids = uint32_t(Model::modelDataArray.size());
        std::cout << "Loading scene:\n";
        std::ifstream sceneFile(path);
        if (sceneFile.is_open()) {
            int lastOriginalIdx = -1; // last one which is not instance of previous
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

                if (meshPath[0] == '!' && lastOriginalIdx != -1) {
                    models.emplace_back(models[lastOriginalIdx], position, scale);
                    models.back().triangleMesh.bbBegin = models[lastOriginalIdx].triangleMesh.bbBegin;
                    models.back().triangleMesh.bbEnd = models[lastOriginalIdx].triangleMesh.bbEnd;

                } else {
                    models.emplace_back(meshPath, position, scale, material);

                    lastOriginalIdx = int(models.size() - 1);
                }

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
    }

    void atFrameBegin(double deltaTime) {
        lights.update(deltaTime);
        updateSceneDataBuffer();
        updateModelData();
        Model::updateBuffers();
        ImGui::SliderInt("lights number", &sceneData.numberOfLights, 0, 20);
        ImGui::SliderInt("recursion depth", &sceneData.recursionDepth, 0, 20);

        directionalShadow.generateShadow(getNumbersOfTriangles());
        pointShadow.generateShadow(getNumbersOfTriangles());

        bvhTree.createFromModels(models);
        bvhTree.bindBuffer(15);
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

    int getNumberOfModels() const {
        return sceneData.endOfModelsAndGrids - sceneData.beginOfModelsAndGrids;
    }

    const auto& getTree() const{
        return bvhTree;
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

    DirectionalShadow directionalShadow;
    PointShadow pointShadow;

    BVHTree bvhTree;
};