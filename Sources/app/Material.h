#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <fstream>
#include <iostream>

#include "Buffer.h"

class Material {
public:
    struct MaterialData {
        glm::vec3 albedo;
        uint32_t type; // 1 - reflection, 0 - refract
        glm::vec3 specular;
        float refractionEta;
    };

    inline static void loadFromFile(const std::string &fileName) {
        pathToFile = fileName;
        load();
    }

    inline static void bindBuffers(uint32_t materialsBufferBinding) {
        materialDataBuffer.createShaderStorageBuffer(materialDataArray, materialsBufferBinding);
    }

    inline static void load(){
        materialDataArray.clear();
        std::ifstream materialsFile(pathToFile);
        if (materialsFile.is_open()) {
            while (true) {
                materialDataArray.push_back(loadOne(materialsFile));
                if (materialsFile.eof()) break;
            }
        }
    }

    inline static void reload(){
        load();
        materialDataBuffer.updateShaderStorageBufferData(materialDataArray);
    }

private:
    inline static MaterialData loadOne(std::istream &in) {
        MaterialData m;
        in >> m.specular.x >> m.specular.y >> m.specular.z;
        in >> m.albedo.x >> m.albedo.y >> m.albedo.z;
        in >> m.type >> m.refractionEta;
        return m;
    }

    inline static std::string pathToFile;
    inline static std::vector<MaterialData> materialDataArray;
    inline static Buffer materialDataBuffer;

};