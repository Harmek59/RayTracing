#pragma once

#include "Model.h"
//axis-aligned bounding box
struct AABB {
    glm::vec3 begin;
    // TODO change modelIndex to gridDDA index
    int modelIndex; // if -1 we did not "hit" model BB yet
    glm::vec3 end;
    uint32_t isValid;
};


class BVHTree {
public:
    BVHTree() = default;

    BVHTree(int numberOFLeafs);

    int getNumberOfNodes() const;

    AABB calculateMaxBB(const std::vector<AABB> &modelsBBs);

    void createFromModels(const std::vector<Model> &modelsArray);

    void recursiveBuild(AABB parent, uint32_t parentIndx, std::vector<AABB> modelsBBs,
                        const std::vector<Model> &modelsArray);

    void createEmpty(int numberOfLeafs);

    void setRoot(AABB value);

    uint32_t insertLeft(AABB value, uint32_t parentNode);

    uint32_t insertRight(AABB value, uint32_t parentNode);

    void bindBuffer(uint32_t treeOfModelBufferBinding);

private:
    std::vector<AABB> treeOfModelIndexes;
    Buffer treeOfModelIndexesBuffer;
    int treeHeight;
    int numberOfNodes;
    bool sahSplit = false;
};

