#include <iostream>
#include "Scene.h"

void Scene::setUp(const std::string &path) {
    sceneData.beginOfModelsAndGrids = uint32_t(GridDDA::getGridDataArray().size());
    std::cout << "========================================\n";
    std::cout << "Loading scene: " << path << "\n";
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

            glm::vec3 rotation;
            sceneFile >> rotation.x >> rotation.y >> rotation.z;

            int material;
            sceneFile >> material;

            if (meshPath[0] == '!' && lastOriginalIdx != -1) {
                models.emplace_back(models[lastOriginalIdx], position, scale, rotation);
            } else {
                models.emplace_back(meshPath, position, scale, rotation, material);

                lastOriginalIdx = int(models.size() - 1);
            }

            if (sceneFile.eof()) break;
        }
    }
    sceneData.endOfModelsAndGrids = uint32_t(GridDDA::getGridDataArray().size());

    TriangleMesh::bindBuffers(8, 9, 7);
    GridDDA::bindBuffers(10, 11, 12);
    Model::bindBuffers(13);

    bindSceneDataBufferBase(5);
}

Scene::Scene(const std::string &path) {
    this->path = path;
    load();
}

void Scene::load() {
    setUp(path);
    bvhTree = BVHTree(getNumbersOfGrids());
}

void Scene::clear() {
    models.clear();
}

void Scene::atFrameBegin() {
    updateSceneDataBuffer();
    updateModelData();
    Model::updateBuffers();

    directionalShadow.generateShadow(models);
    pointShadow.generateShadow(models);

    bvhTree.createFromModels(models);
    bvhTree.bindBuffer(15);
}

void Scene::updateModelData() {
    for (auto &model: models) {
        model.updateModelData();
    }
}

int Scene::getNumbersOfGrids() const {
    int nmbr = 0;
    for (const auto &m: models) {
        nmbr += m.getNumberOfGrids();
    }
    return nmbr;
}

void Scene::bindSceneDataBufferBase(uint32_t bindingPoint) {
    sceneDataBuffer = Buffer(sizeof(SceneData), GL_DYNAMIC_DRAW);
    updateSceneDataBuffer();
    sceneDataBuffer.bindBufferBase(GL_UNIFORM_BUFFER, bindingPoint);
}

void Scene::updateSceneDataBuffer() {
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
