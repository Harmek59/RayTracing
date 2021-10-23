#include "TextureCubeMap.h"
#include <utility>
#include <stb_image.h>
#include <iostream>

TextureCubeMap::TextureCubeMap(uint32_t width, uint32_t height, GLint internalformat, GLenum format, GLenum type,
                               const std::array<uint8_t *, 6> &data) {
    create();
    setTextureParameters();
    setTextureData(width, height, internalformat, format, type, data);
}

TextureCubeMap::TextureCubeMap(const std::array<std::string, 6> &fileNameArr) {
    create();
    setTextureParameters();
    loadDataFromFile(fileNameArr);
}

TextureCubeMap::~TextureCubeMap() {
    remove();
}

TextureCubeMap::TextureCubeMap(TextureCubeMap &&toMove) {
    remove();
    textureID = std::exchange(toMove.textureID, 0);
}

TextureCubeMap &TextureCubeMap::operator=(TextureCubeMap &&toMove) {
    remove();
    textureID = std::exchange(toMove.textureID, 0);
    return *this;
}

void TextureCubeMap::create() {
    glGenTextures(1, &textureID);
}

void TextureCubeMap::setTextureParameters() {
    bind();
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    unBind();
}

void TextureCubeMap::setTextureData(uint32_t width, uint32_t height, GLint internalformat, GLenum format, GLenum type,
                                    const std::array<uint8_t *, 6> &data) {
    bind();
    for (int i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalformat, width, height, 0,
                     format, type, data[i]);
    }
    unBind();

}

void TextureCubeMap::bind() {
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
}

void TextureCubeMap::unBind() {
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

GLuint TextureCubeMap::getTextureID() {
    return textureID;
}

void TextureCubeMap::remove() {
    glDeleteTextures(1, &textureID);
    textureID = 0;
}

void TextureCubeMap::loadDataFromFile(const std::array<std::string, 6> &fileNameArr) {
    bind();
    int width, height, nrChannels;
    GLenum format;
    std::array<uint8_t *, 6> dataArr;
    for (int i = 0; i < 6; i++) {
        unsigned char *data = stbi_load(fileNameArr[0].c_str(), &width, &height, &nrChannels, 0);
        if (nrChannels == 3) format = GL_RGB;
        else if (nrChannels == 4) format = GL_RGBA;
        if (data) {
            dataArr[i] = data;
        } else {
            std::cout << "Failed to load texture: \n" + fileNameArr[i] << std::endl;
        }
    }
    setTextureData(width, height, format, format, GL_UNSIGNED_BYTE, dataArr);
    for (int i = 0; i < 6; i++) {
        stbi_image_free(dataArr[i]);
    }
    unBind();
}

