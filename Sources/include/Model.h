#pragma once

#include "GridDDA.h"
#include "TriangleMesh.h"
#include "DDAGridsCreator.h"

#include <glm/gtc/matrix_transform.hpp>

/* TODO:
 * in GridDDa dataGrid add flag whether draw grid (mesh or not). Every frame check if dataGridArray needs update
 */

class Model {
public:
    Model(const std::string &path, glm::vec3 position, glm::vec3 scale = glm::vec3(1.0f), glm::vec3 rotation = glm::vec3(0.0f), uint32_t materialID = 0) {
        gridsBeginIndex = uint32_t(GridDDA::gridDataArray.size());
        auto[triangleMesh_, gridDataArr, cellsArr] = loadObjMesh(path, position, scale, materialID);

        triangleMesh = std::move(triangleMesh_);

        std::vector<GridDDA> grids;
        std::move(gridDataArr.begin(), gridDataArr.end(), std::back_inserter(grids));

        //MUST BE AFTER CONTRUCTION OF GRID DDA
        std::move(cellsArr.begin(), cellsArr.end(), std::back_inserter(GridDDA::cellsArray));


        this->position = position;
        this->scale = scale;
        this->rotation = rotation;
        auto trianglesBegin = triangleMesh.beginOfMeshTriangles;
        auto trianglesEnd = triangleMesh.endOfMeshTriangles;
        modelDataArray.push_back(
                ModelData{getPositionMatrix(), getRotationMatrix(), getScaleMatrix(), glm::vec4(1.0),
                          glm::ivec4(1), glm::ivec4(1), glm::vec3(1.0),
                          uint32_t(trianglesEnd - trianglesBegin)});
        modelDataIndex = uint32_t(modelDataArray.size() - 1);
        modelDataArray[modelDataIndex].beginEndTriangles = glm::ivec4(trianglesBegin, trianglesEnd, 0, 0);

        for (auto grid: grids) {
            //grid data jest dodawane do gridDataArray w konstruktorze GridDDA
            GridDDA::gridDataArray[grid.gridDataIndex].modelDataId = modelDataIndex;
        }
        gridsEndIndex = uint32_t(GridDDA::gridDataArray.size());

        modelDataArray[modelDataIndex].beginEndGrids = glm::ivec4(gridsBeginIndex, gridsEndIndex, 0, 0);
    }

    Model(const Model &model, glm::vec3 position, glm::vec3 scale, glm::vec3 rotation) {
        this->position = position;
        this->scale = scale;
        this->rotation = rotation;

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

    std::tuple<TriangleMesh, std::vector<GridDDA::GridData>, std::vector<GridDDA::Cell>>
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

    int getNumberOfGrids() const {
        return gridsEndIndex - gridsBeginIndex;
    }

    std::pair<uint32_t, uint32_t> getGridsBeginEndIndexes() const {
        return {gridsBeginIndex, gridsEndIndex};
    }

    int getNumberOfTriangles() const {
        return modelDataArray[modelDataIndex].numberOfTriangles;
    }

//private:
    uint32_t gridsBeginIndex;
    uint32_t gridsEndIndex;

    TriangleMesh triangleMesh;


    struct ModelData {
        glm::mat4 positionMatrix = glm::mat4(1.0);
        glm::mat4 rotationMatrix = glm::mat4(1.0);
        glm::mat4 scaleMatrix = glm::mat4(1.0);
//        bool whetherToDraw;

        glm::vec4 whetherToDraw;
        glm::ivec4 beginEndTriangles;
        glm::ivec4 beginEndGrids;
        glm::vec3 offset2;
        uint32_t numberOfTriangles;

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