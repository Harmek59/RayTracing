#include "Scene.h"

 void Scene::setUp(const std::string &path) {
    sceneData.beginOfModelsAndGrids = uint32_t(Model::modelDataArray.size());
    sceneData.beginOfModelsAndGrids = uint32_t(GridDDA::gridDataArray.size());
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
    sceneData.endOfModelsAndGrids = uint32_t(GridDDA::gridDataArray.size());

    //        std::cout << "Scene summarize:\n";
    //        std::cout << "\tnumber of models: " << sceneData.endOfModelsAndGrids - sceneData.beginOfModelsAndGrids << " ("
    //                  << sceneData.beginOfModelsAndGrids << " - " << sceneData.endOfModelsAndGrids << ")\n";
    //        std::cout << "\tnumber of triangles: " << getNumbersOfTriangles() << " ("
    //                  << Model::modelDataArray[sceneData.beginOfModelsAndGrids].beginEndTriangles.x << " - " <<
    //                  Model::modelDataArray[sceneData.endOfModelsAndGrids - 1].beginEndTriangles.y << ")\n";


    TriangleMesh::bindBuffers(8, 9, 7, 6);
    GridDDA::bindBuffers(10, 11, 12);
    Model::bindBuffers(13);

    bindSceneDataBufferBase(5);
}