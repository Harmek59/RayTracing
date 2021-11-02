#pragma once

#include "GridDDA.h"
#include "TriangleMesh.h"

#include <glm/gtc/matrix_transform.hpp>

/* TODO:
 * in GridDDa dataGrid add flag whether draw grid (mesh or not). Every frame check if dataGridArray needs update
 */

class Model {
public:
    Model(const std::string &path, glm::vec3 position, glm::vec3 scale = glm::vec3(1.0f), uint32_t materialID = 0) {
        gridsBeginIndex = uint32_t(GridDDA::gridDataArray.size());
        GridDDA grid;
        std::tie(triangleMesh, grid) = loadObjMesh(path, position, scale, materialID);

        //take only first half
        int newZSize = grid.gridData.gridResolution.z / 2;
//        grid.gridData.gridResolution.z = newZSize;

        std::vector<GridDDA> grids;
        grids.push_back(grid);
        this->position = position;
        this->scale = scale;
        auto trianglesBegin = std::distance(triangleMesh.triangles.begin(), triangleMesh.beginOfMeshTriangles);
        auto trianglesEnd = std::distance(triangleMesh.triangles.begin(), triangleMesh.endOfMeshTriangles);
        modelDataArray.push_back(
                ModelData{getPositionMatrix(), getRotationMatrix(), getScaleMatrix(), glm::vec4(1.0), glm::ivec4(1),
                          uint32_t(trianglesEnd - trianglesBegin), glm::vec3(1.0), glm::vec4(1.0)});
        modelDataIndex = uint32_t(modelDataArray.size() - 1);
        modelDataArray[modelDataIndex].beginEndTriangles = glm::ivec4(trianglesBegin, trianglesEnd, 0, 0);

        for (auto grid: grids) {
            //grid data jest dodawane do gridDataArray w konstruktorze GridDDA
            GridDDA::gridDataArray[grid.gridDataIndex].modelDataId = modelDataIndex;

//            auto gd = GridDDA::gridDataArray[grid.gridDataIndex];// second one
//
//            GridDDA::gridDataArray[grid.gridDataIndex].gridResolution.z = newZSize;
//
//            GridDDA::gridDataArray[grid.gridDataIndex].gridEnd.z = GridDDA::gridDataArray[grid.gridDataIndex].gridBegin.z + grid.gridData.cellSize.z * newZSize;
//            GridDDA::gridDataArray[grid.gridDataIndex].cellsEndIndex =
//                    GridDDA::gridDataArray[grid.gridDataIndex].cellsBeginIndex +
//                    newZSize * grid.gridData.gridResolution.y * grid.gridData.gridResolution.x;
//
//
//            gd.gridResolution.z = gd.gridResolution.z - newZSize -1;
//            gd.gridBegin.z += grid.gridData.cellSize.z * gd.gridResolution.z;
//            gd.cellsBeginIndex += gd.gridResolution.z * grid.gridData.gridResolution.y * grid.gridData.gridResolution.x;
//            GridDDA::gridDataArray.push_back(gd);
        }
        gridsEndIndex = uint32_t(GridDDA::gridDataArray.size());
    }

    Model(const Model &model, glm::vec3 position, glm::vec3 scale) {
        this->position = position;
        this->scale = scale;

        ModelData modelData = modelDataArray[model.modelDataIndex];
        modelData.positionMatrix = getPositionMatrix();
        modelData.scaleMatrix = getScaleMatrix();
        modelDataArray.push_back(modelData);
        modelDataIndex = uint32_t(modelDataArray.size() - 1);
//        grids = model.grids;
        triangleMesh = model.triangleMesh;


        gridsBeginIndex = uint32_t(GridDDA::gridDataArray.size());
        std::vector<GridDDA::GridData> temp;


        for (auto i = model.gridsBeginIndex; i < model.gridsEndIndex; i++) {
            temp.push_back(GridDDA::gridDataArray[i]);
            temp.back().modelDataId = modelDataIndex;
        }

        std::move(temp.begin(), temp.end(), std::back_inserter(GridDDA::gridDataArray));
        gridsEndIndex = uint32_t(GridDDA::gridDataArray.size());
    }

    std::pair<TriangleMesh, GridDDA>
    loadObjMesh(const std::string &path, glm::vec3 position, glm::vec3 multiplier, uint32_t materialID);

    glm::mat4 getModelMatrix() const;

    glm::mat4 getPositionMatrix() const;

    glm::mat4 getRotationMatrix() const;

    glm::mat4 getScaleMatrix() const;

    void updateModelData();

    static void
    bindBuffers(uint32_t modelDataBufferBinding) {
        bindOneBuffer(modelDataArrayBuffer, modelDataArray, modelDataBufferBinding);
    }

    static void updateBuffers() {
        modelDataArrayBuffer.bind(GL_SHADER_STORAGE_BUFFER);
        ModelData *dataPtr = (ModelData *) modelDataArrayBuffer.mapBuffer(GL_SHADER_STORAGE_BUFFER,
                                                                          GL_WRITE_ONLY | GL_MAP_INVALIDATE_BUFFER_BIT);
        if (!dataPtr) {
            throw std::runtime_error("Model error during update function: mapping buffer failed");
        }
        std::memcpy(dataPtr, modelDataArray.data(), modelDataArray.size() * sizeof(ModelData));

        modelDataArrayBuffer.unMap(GL_SHADER_STORAGE_BUFFER);
        modelDataArrayBuffer.unBind();
    }

//private:
    uint32_t gridsBeginIndex;
    uint32_t gridsEndIndex;

//    GridDDA grid;
    TriangleMesh triangleMesh;


    struct ModelData {
        glm::mat4 positionMatrix = glm::mat4(1.0);
        glm::mat4 rotationMatrix = glm::mat4(1.0);
        glm::mat4 scaleMatrix = glm::mat4(1.0);
//        bool whetherToDraw;

        glm::vec4 whetherToDraw;
        glm::ivec4 beginEndTriangles;
        uint32_t numberOfTriangles;
        glm::vec3 offset;
        glm::vec4 offset2;
    };

    static inline std::vector<ModelData> modelDataArray;
    static inline Buffer modelDataArrayBuffer;

    uint32_t modelDataIndex;
    glm::vec3 position = glm::vec3(0.0);
    glm::vec3 rotation = glm::vec3(0.0);
    glm::vec3 scale = glm::vec3(1.0);
    bool draw = true;

    template<typename dataType>
    static void bindOneBuffer(Buffer &buff, dataType &data, uint32_t bindingBlock) {
        using Type = typename dataType::value_type;
        buff = Buffer(data.size() * sizeof(Type), GL_DYNAMIC_DRAW);
        buff.bind(GL_SHADER_STORAGE_BUFFER);
        Type *dataPtr = (Type *) buff.mapBuffer(GL_SHADER_STORAGE_BUFFER,
                                                GL_WRITE_ONLY | GL_MAP_INVALIDATE_BUFFER_BIT);
        if (!dataPtr) {
            throw std::runtime_error("Model error during update function: mapping buffer failed");
        }
        std::memcpy(dataPtr, data.data(), data.size() * sizeof(Type));

        buff.unMap(GL_SHADER_STORAGE_BUFFER);
        buff.unBind();

        buff.bindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingBlock);
    }

};