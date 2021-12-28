#pragma once

#include <fstream>

#include "Model.h"
#include "Buffer.h"
#include "FrameBuffer.h"
#include "DirectionalShadow.h"
#include "PointShadow.h"
#include "BVHTree.h"
#include <imgui.h>


class Scene {
public:
    Scene(const std::string &path);

    void setUp(const std::string &path);

    void load();

    void clear();

    void atFrameBegin();

    inline auto &getModels() {
        return models;
    }

    inline const auto &getModels() const {
        return models;
    }

    void updateModelData();

    int getNumbersOfGrids() const;

    inline const auto &getTree() const {
        return bvhTree;
    }

    inline const auto &getDirectionalLightMatrix() const {
        return directionalShadow.getLightSpaceMatrix();
    }

    inline auto &getPointLightPositionRef() {
        return pointShadow.getLightPositionRef();
    }

    inline auto &getPointLightRageRef() {
        return pointShadow.getLightRangeRef();
    }

private:
    void bindSceneDataBufferBase(uint32_t bindingPoint);

    void updateSceneDataBuffer();

    struct SceneData {
        uint32_t beginOfModelsAndGrids = 0;
        uint32_t endOfModelsAndGrids = 0;

    } sceneData;
    static inline Buffer sceneDataBuffer;


    std::vector<Model> models;

    DirectionalShadow directionalShadow;
    PointShadow pointShadow;

    BVHTree bvhTree;

    std::string path;
};