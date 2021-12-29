#include "Model.h"
#include <tiny_obj_loader.h>
#include <iostream>

#include "DDAGridsCreatorAndSplitter.h"

glm::mat4 Model::getPositionMatrix() const {
    glm::mat4 model = glm::mat4(1.);
    model = glm::translate(model, position);
    return model;
}

glm::mat4 Model::getRotationMatrix() const {
    glm::mat4 model = glm::mat4(1.);
    model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1., 0., 0.));
    model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0., 1., 0.));
    model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0., 0., 1.));
    return model;
}

glm::mat4 Model::getScaleMatrix() const {
    glm::mat4 model = glm::mat4(1.);
    model = glm::scale(model, scale);
    return model;
}

void Model::updateModelData() {
    modelDataArray[modelDataIndex].positionMatrix = getPositionMatrix();
    modelDataArray[modelDataIndex].rotationMatrix = getRotationMatrix();
    modelDataArray[modelDataIndex].scaleMatrix = getScaleMatrix();

    if (draw) {
        modelDataArray[modelDataIndex].whetherToDraw[0] = 1;
    } else {
        modelDataArray[modelDataIndex].whetherToDraw[0] = 0;
    }
}

std::tuple<TriangleMesh, std::vector<GridDDA::GridData>, std::vector<GridDDA::Cell>, std::vector<uint32_t>>
Model::loadObjMesh(const std::string &path, glm::vec3 position, glm::vec3 multiplier, uint32_t materialID) {

    std::cout << "Loading obj mesh: " << path << '\n';
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str())) {
        std::cout << warn + err << std::endl;
        throw std::runtime_error(warn + err);
    }
    std::vector<TriangleMesh::Vertex> vertices;
    std::vector<TriangleMesh::Triangle> triangles;
    std::vector<TriangleMesh::Normals> normals;

    for (auto iter = attrib.vertices.begin(); iter < attrib.vertices.end(); iter += 3) {
        TriangleMesh::Vertex vert;
        vert.x = *iter;
        vert.y = *std::next(iter);
        vert.z = *std::next(iter, 2);

        vertices.push_back(vert);
    }

    for (auto iter = attrib.normals.begin(); iter < attrib.normals.end(); iter += 3) {
        TriangleMesh::Normals normal;
        normal.normal = glm::normalize(glm::vec3(*iter, *std::next(iter), *std::next(iter, 2)));

        normals.push_back(std::move(normal));
    }

    for (const auto &shape : shapes) {
        for (unsigned i = 0; i < shape.mesh.indices.size(); i += 3) {


            TriangleMesh::Triangle triangle;
            triangle.vertexIndex1 = shape.mesh.indices[i].vertex_index;
            triangle.vertexIndex2 = shape.mesh.indices[i + 1].vertex_index;
            triangle.vertexIndex3 = shape.mesh.indices[i + 2].vertex_index;
            triangle.material = materialID; //shape.mesh.material_ids[i];



            triangle.normalIndex1 = shape.mesh.indices[i].normal_index;
            triangle.normalIndex2 = shape.mesh.indices[i + 1].normal_index;
            triangle.normalIndex3 = shape.mesh.indices[i + 2].normal_index;

            triangles.push_back(triangle);
        }
    }


    TriangleMesh triangleMesh(std::move(vertices), std::move(triangles), std::move(normals));
    triangleMesh.normalize();

    DDAGridsCreatorAndSplitter gridsCreator;
    auto[gridData, cellsArr, triangleIndicesArr] = gridsCreator.calculateBaseGridAndCells(triangleMesh);
    auto[splittedGridDataArr, splittedCellsArr] = gridsCreator.baseGridSplitter(gridData, cellsArr);
    return std::make_tuple<TriangleMesh, std::vector<GridDDA::GridData>, std::vector<GridDDA::Cell>, std::vector<uint32_t>>(
            std::move(triangleMesh),
            std::move(splittedGridDataArr), std::move(splittedCellsArr), std::move(triangleIndicesArr));


}

Model::Model(const std::string &path, glm::vec3 position, glm::vec3 scale, glm::vec3 rotation, uint32_t materialID) {
    gridsBeginIndex = uint32_t(GridDDA::getGridDataArray().size());
    auto[triangleMesh_, gridDataArr, cellsArr, triangleIndicesArr] = loadObjMesh(path, position, scale, materialID);
    triangleMesh = std::move(triangleMesh_);

    std::vector<GridDDA> grids;
    std::move(gridDataArr.begin(), gridDataArr.end(), std::back_inserter(grids));

    // must be after construction of grid DDA
    GridDDA::addToCellArrAndTrianglesIndicesArr(cellsArr, triangleIndicesArr);


    this->position = position;
    this->scale = scale;
    this->rotation = rotation;
    auto trianglesBegin = triangleMesh.getBeginOfMeshTriangles();
    auto trianglesEnd = triangleMesh.getEndOfMeshTriangles();
    modelDataArray.push_back(
            ModelData{getPositionMatrix(), getRotationMatrix(), getScaleMatrix(), glm::vec4(1.0),
                      glm::ivec4(1), glm::ivec4(1), glm::vec3(1.0),
                      uint32_t(trianglesEnd - trianglesBegin)});
    modelDataIndex = uint32_t(modelDataArray.size() - 1);
    modelDataArray[modelDataIndex].beginEndTriangles = glm::ivec4(trianglesBegin, trianglesEnd, 0, 0);

    for (auto grid: grids) {
        GridDDA::getGridDataArray()[grid.getGridDataIndex()].modelDataId = modelDataIndex;
    }
    gridsEndIndex = uint32_t(GridDDA::getGridDataArray().size());

    modelDataArray[modelDataIndex].beginEndGrids = glm::ivec4(gridsBeginIndex, gridsEndIndex, 0, 0);
}

Model::Model(const Model &model, glm::vec3 position, glm::vec3 scale, glm::vec3 rotation) {
    this->position = position;
    this->scale = scale;
    this->rotation = rotation;

    ModelData modelData = modelDataArray[model.modelDataIndex];
    modelData.positionMatrix = getPositionMatrix();
    modelData.scaleMatrix = getScaleMatrix();
    modelDataArray.push_back(modelData);
    modelDataIndex = uint32_t(modelDataArray.size() - 1);

    triangleMesh = model.triangleMesh;


    gridsBeginIndex = uint32_t(GridDDA::getGridDataArray().size());
    std::vector<GridDDA::GridData> temp;


    for (auto i = model.gridsBeginIndex; i < model.gridsEndIndex; i++) {
        temp.push_back(GridDDA::getGridDataArray()[i]);
        temp.back().modelDataId = modelDataIndex;
    }

    std::move(temp.begin(), temp.end(), std::back_inserter(GridDDA::getGridDataArray()));
    gridsEndIndex = uint32_t(GridDDA::getGridDataArray().size());

    triangleMesh = model.triangleMesh;
}

void Model::bindBuffers(uint32_t modelDataBufferBinding) {
    modelDataArrayBuffer.createShaderStorageBuffer(modelDataArray, modelDataBufferBinding);
}

void Model::updateBuffers() {
    modelDataArrayBuffer.updateShaderStorageBufferData(modelDataArray);
}

void Model::clear() {
    modelDataArray.clear();
}

int Model::getNumberOfGrids() const {
    return gridsEndIndex - gridsBeginIndex;
}

std::pair<uint32_t, uint32_t> Model::getGridsBeginEndIndexes() const {
    return {gridsBeginIndex, gridsEndIndex};
}

int Model::getNumberOfTriangles() const {
    return modelDataArray[modelDataIndex].numberOfTriangles;
}

uint32_t Model::getModelDataIndex() const {
    return modelDataIndex;
}

glm::vec3 &Model::getPositionRef() {
    return position;
}

glm::vec3 &Model::getRotationRef() {
    return rotation;
}

glm::vec3 &Model::getScaleRef() {
    return scale;
}

bool &Model::getDrawRef() {
    return draw;
}

uint32_t Model::getGridsBeginIndex() const {
    return gridsBeginIndex;
}

uint32_t Model::getGridsEndIndex() const {
    return gridsEndIndex;
}
