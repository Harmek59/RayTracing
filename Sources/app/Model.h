#pragma once

#include "GridDDA.h"
#include "TriangleMesh.h"
#include "DDAGridsCreatorAndSplitter.h"

#include <glm/gtc/matrix_transform.hpp>
#include <iterator>

class Model {
public:
    struct ModelData {
        glm::mat4 positionMatrix = glm::mat4(1.0);
        glm::mat4 rotationMatrix = glm::mat4(1.0);
        glm::mat4 scaleMatrix = glm::mat4(1.0);

        glm::vec4 whetherToDraw;
        glm::ivec4 beginEndTriangles;
        glm::ivec4 beginEndGrids;
        glm::vec3 offset2;
        uint32_t numberOfTriangles;

    };

    Model(const std::string &path, glm::vec3 position, glm::vec3 scale, glm::vec3 rotation, uint32_t materialID);

    Model(const Model &model, glm::vec3 position, glm::vec3 scale, glm::vec3 rotation);

    std::tuple<TriangleMesh, std::vector<GridDDA::GridData>, std::vector<GridDDA::Cell>, std::vector<uint32_t>>
    loadObjMesh(const std::string &path, glm::vec3 position, glm::vec3 multiplier, uint32_t materialID);

    glm::mat4 getPositionMatrix() const;

    glm::mat4 getRotationMatrix() const;

    glm::mat4 getScaleMatrix() const;

    void updateModelData();

    static void bindBuffers(uint32_t modelDataBufferBinding);

    static void updateBuffers();

    static void clear();

    int getNumberOfGrids() const;

    std::pair<uint32_t, uint32_t> getGridsBeginEndIndexes() const;

    int getNumberOfTriangles() const;

    uint32_t getModelDataIndex() const;

    glm::vec3 &getPositionRef();

    glm::vec3 &getRotationRef();

    glm::vec3 &getScaleRef();

    bool &getDrawRef();

    uint32_t getGridsBeginIndex() const;

    uint32_t getGridsEndIndex() const;

private:
    uint32_t gridsBeginIndex;
    uint32_t gridsEndIndex;

    TriangleMesh triangleMesh;

    static inline std::vector<ModelData> modelDataArray;
    static inline Buffer modelDataArrayBuffer;

    uint32_t modelDataIndex;
    glm::vec3 position = glm::vec3(0.0);
    glm::vec3 rotation = glm::vec3(0.0);
    glm::vec3 scale = glm::vec3(1.0);
    bool draw = true;


};