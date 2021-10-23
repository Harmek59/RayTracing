#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <fstream>

#include "Buffer.h"

class Material {
public:
    struct MaterialData {
        glm::vec3 albedo;
        uint32_t offset1;
        glm::vec3 specular;
        uint32_t offset2;
    };

    void loadFromFile(const std::string &fileName) {
        std::ifstream materialsFile(fileName);
        if (materialsFile.is_open()) {
            while (true) {
                materialDataArray.push_back(loadOne(materialsFile));
                if (materialsFile.eof()) break;
            }
        }
    }

    static void bindBuffers(uint32_t materialsBufferBinding) {
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
        std::cout<<"materialDataArray.size(): "<<materialDataArray.size()<<std::endl;
        bindBuffer(materialDataBuffer, materialDataArray, materialsBufferBinding);
    }

private:
    MaterialData loadOne(std::istream &in) {
        MaterialData m;
        in >> m.specular.x >> m.specular.y >> m.specular.z;
        in >> m.albedo.x >> m.albedo.y >> m.albedo.z;
        return m;
    }

    inline static std::vector<MaterialData> materialDataArray;
    inline static Buffer materialDataBuffer;

};