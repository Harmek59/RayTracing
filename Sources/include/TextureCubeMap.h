#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <array>
#include <string>

class TextureCubeMap {
public:
    TextureCubeMap(uint32_t width, uint32_t height, GLint internalformat, GLenum format, GLenum type,
                   const std::array<uint8_t *, 6> &data = {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr});

    TextureCubeMap(const std::array<std::string,6> &fileNameArr);

    ~TextureCubeMap();

    TextureCubeMap(TextureCubeMap &&);

    TextureCubeMap &operator=(TextureCubeMap &&);

    //delete copy
    TextureCubeMap(const TextureCubeMap &) = delete;

    TextureCubeMap &operator=(const TextureCubeMap &) = delete;

    void create();

    void setTextureParameters();

    void setTextureData(uint32_t width, uint32_t height, GLint internalformat, GLenum format, GLenum type,
                        const std::array<uint8_t *, 6> &data);

//    void generateMipMap();

    void loadDataFromFile(const std::array<std::string,6> &fileNameArr);

    void bind();

    void unBind();

    GLuint getTextureID();

    void remove();

private:
    GLuint textureID = 0;
};