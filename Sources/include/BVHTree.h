#pragma once

#include <vector>
#include <cmath>
#include <numeric>
#include <chrono>
#include "Model.h"

//axis-aligned bounding box
struct AABB {
    glm::vec3 begin;
    // TODO change modelIndex to gridDDA index
    int modelIndex; // if -1 we did not "hit" model BB yet
    glm::vec3 end;
    uint32_t isValid;
};

#include <glm/gtx/string_cast.hpp>

std::ostream &operator<<(std::ostream &out, const glm::vec3 &vec) {
    out << glm::to_string(vec);
    return out;
}


class BVHTree {
public:
    BVHTree() = default;

    BVHTree(int numberOFLeafs) {
        createEmpty(numberOFLeafs * 10);
    }

    int getNumberOfNodes() const {
        return numberOfNodes;
    }

    AABB calculateMaxBB(const std::vector<AABB> &modelsBBs) {
        AABB bb = AABB{glm::vec3(99999.), -1, glm::vec3(-99999.), 1}; // TODO change to max/minFloatVal

        for (auto &m : modelsBBs) {
            bb.begin.x = std::min(bb.begin.x, m.begin.x);
            bb.begin.y = std::min(bb.begin.y, m.begin.y);
            bb.begin.z = std::min(bb.begin.z, m.begin.z);

            bb.end.x = std::max(bb.end.x, m.end.x);
            bb.end.y = std::max(bb.end.y, m.end.y);
            bb.end.z = std::max(bb.end.z, m.end.z);
        }

        return bb;
    }

    void createFromModels(const std::vector<Model> &modelsArray) {
        ImGui::Checkbox("SAH split", &sahSplit);

        // Get starting timepoint
        auto start = std::chrono::high_resolution_clock::now();

        std::vector<AABB> modelsBB;
        modelsBB.reserve(modelsArray.size());
        for (int i = 0; i < modelsArray.size(); ++i) {
            glm::mat4 modelMatrix = glm::mat4(1.0f);
            modelMatrix = modelsArray[i].getPositionMatrix() * modelsArray[i].getRotationMatrix() *
                          modelsArray[i].getScaleMatrix();

            for(int j = modelsArray[i].gridsBeginIndex; j <modelsArray[i].gridsEndIndex; j++){
                auto bbMin = GridDDA::gridDataArray[j].gridBegin;
                auto bbMax = GridDDA::gridDataArray[j].gridEnd;

                //https://gamedev.stackexchange.com/questions/162819/how-do-axis-aligned-bounding-boxes-update-with-rotations
                // to jest niedokładne bb jest za duży po obrocie (aby był perfekcyjny trzeba obliczyć od początku
                glm::vec3 corners[8];
                corners[0] = bbMin;
                corners[1] = {bbMin.x, bbMin.y, bbMax.z};
                corners[2] = {bbMin.x, bbMax.y, bbMin.z};
                corners[3] = {bbMax.x, bbMin.y, bbMin.z};
                corners[4] = {bbMin.x, bbMax.y, bbMax.z};
                corners[5] = {bbMax.x, bbMin.y, bbMax.z};
                corners[6] = {bbMax.x, bbMax.y, bbMin.z};
                corners[7] = bbMax;

                // TODO change to max/minFloatVal
                glm::vec3 min = glm::vec3(99999.);
                glm::vec3 max = glm::vec3(-99999.);

                for (int i = 0; i < 8; i++) {
                    glm::vec3 transformed = modelMatrix * glm::vec4(corners[i], 1.0f);
                    min.x = std::min(min.x, transformed.x);
                    min.y = std::min(min.y, transformed.y);
                    min.z = std::min(min.z, transformed.z);

                    max.x = std::max(max.x, transformed.x);
                    max.y = std::max(max.y, transformed.y);
                    max.z = std::max(max.z, transformed.z);
                }

                bbMin = min;
                bbMax = max;
                modelsBB.push_back(AABB{bbMin, j, bbMax, 1});
            }
        }
        AABB rootBB = calculateMaxBB(modelsBB);

        treeOfModelIndexes[0] = rootBB;
        if(modelsArray.size() == 1){
            treeOfModelIndexes[0].modelIndex = 0;
        }else{
            recursiveBuild(rootBB, 0, modelsBB, modelsArray);
        }

        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);

        ImGui::Text("BVH tree creation time: %f ms", duration.count() / 1000.);
    }

    void recursiveBuild(AABB parent, uint32_t parentIndx, std::vector<AABB> modelsBBs,
                        const std::vector<Model> &modelsArray) {
        auto findLongestAxis = [](const AABB &bb) {
            glm::vec3 size = bb.end - bb.begin;
            int maxIndex = 0;
            for (int i = 0; i < 3; i++) {
                if (size[maxIndex] < size[i]) {
                    size[maxIndex] = size[i];
                    maxIndex = i;
                }
            }
            return maxIndex;
        };
        auto longestAxis = findLongestAxis(parent);
//        std::cout << "RecursiveBuild:\nparent begin: " << parent.begin << ", parent end: " << parent.end
//                  << " | max Axis: " << longestAxis << "\n";


        std::sort(modelsBBs.begin(), modelsBBs.end(), [longestAxis](const AABB &b1, const AABB &b2) {
            return b1.end[longestAxis] < b2.end[longestAxis];
        });


        int splitPoint = int(modelsBBs.size() / 2);
        // calculate split Point: ( SAH )
        if (sahSplit) {
            using Split = std::pair<float, std::pair<AABB, AABB>>;
            std::vector<Split> splits;

            auto mergeAABB = [](const AABB &bb1, const AABB &bb2) {
                AABB resultBB;
                resultBB.end.x = std::max(bb1.end.x, bb2.end.x);
                resultBB.end.y = std::max(bb1.end.y, bb2.end.y);
                resultBB.end.z = std::max(bb1.end.z, bb2.end.z);

                resultBB.begin.x = std::min(bb1.begin.x, bb2.begin.x);
                resultBB.begin.y = std::min(bb1.begin.y, bb2.begin.y);
                resultBB.begin.z = std::min(bb1.begin.z, bb2.begin.z);

                return resultBB;
            };
//            std::cout << "==========\n";
//            std::cout << parent.begin << " | " << parent.end << "\n";

            // Compute costs for splitting after each bucket
            for (int splitAfter = 0; splitAfter < modelsBBs.size() - 1; splitAfter++) {
                //dont want to check (for now) what if we split after all models(left split all models, right split zero)
                AABB left = modelsBBs[0];

                for (int i = 1; i <= splitAfter; i++) {
                    left = mergeAABB(left, modelsBBs[i]);
                }
//                float leftNumberOfCells =
//                        std::pow((left.end.x - left.begin.x), 2) + std::pow((left.end.y - left.begin.y), 2) +
//                        std::pow((left.end.z - left.begin.z), 2);
                float leftNumberOfCells =
                        (left.end.x - left.begin.x) * (left.end.y - left.begin.y) * (left.end.z - left.begin.z);


                AABB right = modelsBBs[splitAfter + 1];

                for (int i = splitAfter + 2; i < modelsBBs.size(); i++) {
                    right = mergeAABB(right, modelsBBs[i]);

                }
//                float rightNumberOfCells =
//                        std::pow((right.end.x - right.begin.x), 2) + std::pow((right.end.y - right.begin.y), 2) +
//                        std::pow((right.end.z - right.begin.z), 2);
                float rightNumberOfCells =
                        (right.end.x - right.begin.x) * (right.end.y - right.begin.y) * (right.end.z - right.begin.z);
//                std::cout << "modelsBBs.size(): " << modelsBBs.size() << " dziele po " << splitAfter << ": "
//                          << leftNumberOfCells + rightNumberOfCells << " left: " << leftNumberOfCells << " | right: "
//                          << rightNumberOfCells << "\n\tleftAABB:" << left.begin << " | "
//                          << left.end << " \n\trightAABB: " << right.begin << " | " << right.end << std::endl;

                splits.push_back(Split{leftNumberOfCells + rightNumberOfCells, {left, right}});
            }

            int bestSplit = 0;
            for (int i = 1; i < modelsBBs.size() - 1; i++) {
                if (splits[bestSplit].first >= splits[i].first) {
                    if (splits[bestSplit].first == splits[i].first) {
                        // if i is closer to model center
                        if (std::abs(int(i - modelsBBs.size() / 2)) < std::abs(int(bestSplit - modelsBBs.size() / 2))) {
                            bestSplit = i;
                        }
                    } else {
                        bestSplit = i;
                    }
                }
            }
            splitPoint = bestSplit + 1;
        }

        //

        std::vector<AABB> split1 = {modelsBBs.begin(), std::next(modelsBBs.begin(), splitPoint)};
        if (split1.size() == 1) {
            insertLeft(split1.front(), parentIndx);

        } else {
            AABB split1BB = calculateMaxBB(split1);
            recursiveBuild(split1BB, insertLeft(split1BB, parentIndx), std::move(split1), modelsArray);
        }


        std::vector<AABB> split2 = {std::next(modelsBBs.begin(), splitPoint), modelsBBs.end()};
        if (split2.size() == 1) {
            insertRight(split2.front(), parentIndx);

        } else {
            AABB split2BB = calculateMaxBB(split2);
            recursiveBuild(split2BB, insertRight(split2BB, parentIndx), std::move(split2), modelsArray);
        }


    }

    void createEmpty(int numberOfLeafs) {
        treeHeight = int(std::ceil(std::log2(numberOfLeafs)));
        numberOfNodes = int(std::pow(2, treeHeight + 1) - 1);
        treeOfModelIndexes = std::vector<AABB>(numberOfNodes,
                                               AABB{glm::vec3{0.0f, 0.0f, 0.0f}, -1, glm::vec3{0.0f, 0.0f, 0.0f}, 0});
        std::cout << numberOfNodes << std::endl;
    }


    void setRoot(AABB value) {
        if (treeOfModelIndexes.size() > 0) {
            treeOfModelIndexes[0] = value;
        }
    }

    uint32_t insertLeft(AABB value, uint32_t parentNode) {
        uint32_t insertPos = 2 * parentNode + 1;
        if (insertPos < treeOfModelIndexes.size()) {
            treeOfModelIndexes[insertPos] = value;
        } else {
            std::cout << "Drzewo za male!\n";
        }
        return insertPos;
    }

    uint32_t insertRight(AABB value, uint32_t parentNode) {
        uint32_t insertPos = 2 * parentNode + 2;
        if (insertPos < treeOfModelIndexes.size()) {
            treeOfModelIndexes[insertPos] = value;
        } else {
            std::cout << "Drzewo za male!\n";
        }
        return insertPos;
    }

    void bindBuffer(uint32_t treeOfModelBufferBinding) {
        auto bindBuffer = [](Buffer &buff, auto &data, uint32_t bindingBlock) {
            using Type = std::decay_t<decltype(data)>::value_type;
            buff = Buffer(data.size() * sizeof(Type), GL_DYNAMIC_DRAW);
            buff.bind(GL_SHADER_STORAGE_BUFFER);
            Type *dataPtr = (Type *) buff.mapBuffer(GL_SHADER_STORAGE_BUFFER,
                                                    GL_WRITE_ONLY | GL_MAP_INVALIDATE_BUFFER_BIT);
            if (!dataPtr) {
                throw std::runtime_error("Temp_Spheres error during update function: mapping buffer failed");
            }
            std::memcpy(dataPtr, data.data(), data.size() * sizeof(Type));

            buff.unMap(GL_SHADER_STORAGE_BUFFER);
            buff.unBind();

            buff.bindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingBlock);
        };
        bindBuffer(treeOfModelIndexesBuffer, treeOfModelIndexes, treeOfModelBufferBinding);
    }

private:
    std::vector<AABB> treeOfModelIndexes;
    Buffer treeOfModelIndexesBuffer;
    int treeHeight;
    int numberOfNodes;
    bool sahSplit = false;
};

