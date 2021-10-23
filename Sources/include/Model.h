#pragma once

#include "GridDDA.h"
#include "TriangleMesh.h"
#include "LoadMeshFromFile.h"

#include <glm/gtc/matrix_transform.hpp>

/* TODO:
 * in GridDDa dataGrid add flag whether draw grid (mesh or not). Every frame check if dataGridArray needs update
 */

class Model {
public:
    Model(const std::string &path, glm::vec3 position, glm::vec3 scale = glm::vec3(1.0f), uint32_t materialID = 0) {
        std::tie(triangleMesh, grid) = loadObjMesh(path, position, scale, materialID);
        this->position = position;
        this->scale = scale;
        auto trianglesBegin = std::distance(triangleMesh.triangles.begin(), triangleMesh.beginOfMeshTriangles);
        auto trianglesEnd = std::distance(triangleMesh.triangles.begin(), triangleMesh.endOfMeshTriangles);
        modelDataArray.push_back(
                ModelData{getPositionMatrix(), getRotationMatrix(), getScaleMatrix(), glm::vec4(1.0), glm::ivec4(1),
                          uint32_t(trianglesEnd - trianglesBegin), glm::vec3(1.0), glm::vec4(1.0)});
        modelDataIndex = uint32_t(modelDataArray.size() - 1);
        modelDataArray[modelDataIndex].beginEndTriangles = glm::ivec4(trianglesBegin, trianglesEnd, 0, 0);
    }

    Model(const Model &model, glm::vec3 position, glm::vec3 scale) {
        this->position = position;
        this->scale = scale;
        GridDDA::gridDataArray.push_back(model.grid.getGridData());
        ModelData modelData = modelDataArray[model.modelDataIndex];
        modelData.positionMatrix = getPositionMatrix();
        modelData.scaleMatrix = getScaleMatrix();
        modelDataArray.push_back(modelData);
        modelDataIndex = uint32_t(modelDataArray.size() - 1);
    }

    std::pair<TriangleMesh, GridDDA>
    loadObjMesh(const std::string &path, glm::vec3 position, glm::vec3 multiplier, uint32_t materialID);

    glm::mat4 getModelMatrix() const {
        glm::mat4 model = glm::mat4(1.);
        model = glm::translate(model, position);
        model = glm::scale(model, scale);
        return model;
    }

    glm::mat4 getPositionMatrix() const {
        glm::mat4 model = glm::mat4(1.);
        model = glm::translate(model, position);
        return model;
    }

    glm::mat4 getRotationMatrix() const {
        glm::mat4 model = glm::mat4(1.);
        model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1., 0., 0.));
        model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0., 1., 0.));
        model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0., 0., 1.));
        return model;
    }

    glm::mat4 getScaleMatrix() const {
        glm::mat4 model = glm::mat4(1.);
        model = glm::scale(model, scale);
        return model;
    }

    void updateModelData() {
        modelDataArray[modelDataIndex].positionMatrix = getPositionMatrix();
        modelDataArray[modelDataIndex].rotationMatrix = getRotationMatrix();
        modelDataArray[modelDataIndex].scaleMatrix = getScaleMatrix();

        if (draw) {
            modelDataArray[modelDataIndex].whetherToDraw[0] = 1;
        } else {
            modelDataArray[modelDataIndex].whetherToDraw[0] = 0;
        }
    }

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
    GridDDA grid;
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
        using Type = dataType::value_type;
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