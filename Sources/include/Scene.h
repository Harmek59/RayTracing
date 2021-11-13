#pragma once

#include <fstream>

#include "Model.h"
#include "Buffer.h"
#include "FrameBuffer.h"
#include "DirectionalSadow.h"
#include "PointShadow.h"
#include "BVHTree.h"
#include <imgui.h>


class Scene {
public:

    Scene(const std::string &path) {
        this->path = path;
        load();
    }

    void setUp(const std::string &path);

    void load(){
        setUp(path);
        bvhTree = BVHTree(getNumbersOfGrids());
    }

    void clear(){
        models.clear();
    }

    void atFrameBegin(double deltaTime) {
        updateSceneDataBuffer();
        updateModelData();
        Model::updateBuffers();

        directionalShadow.generateShadow(models);
        pointShadow.generateShadow(models);

        bvhTree.createFromModels(models);
        bvhTree.bindBuffer(15);
    }

    auto &getModels() {
        return models;
    }

    const auto &getModels() const {
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
            for (auto i = m.gridsBeginIndex; i < m.gridsEndIndex; i++) {
                const auto &grid = GridDDA::gridDataArray[i];
                nmbr += grid.cellsEndIndex - grid.cellsBeginIndex;
            }
        }
        return nmbr;
    }

    int getNumbersOfGrids() const {
        int nmbr = 0;
        for (const auto &m: models) {
            nmbr += m.getNumberOfGrids();
        }
        return nmbr;
    }

//    int getNumberOfModels() const {
//        return sceneData.endOfModelsAndGrids - sceneData.beginOfModelsAndGrids;
//    }

    const auto &getTree() const {
        return bvhTree;
    }

private:
    struct SceneData {
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

    DirectionalShadow directionalShadow;
    PointShadow pointShadow;

    BVHTree bvhTree;

    std::string path;
};