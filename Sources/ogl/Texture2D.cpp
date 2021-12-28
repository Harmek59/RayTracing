#include "Texture2D.h"

#include <stb_image.h>

#include <utility>
#include <iostream>

Texture2D::Texture2D() : TextureInterface(TextureInterface::Construction::NoCreate) {
}

Texture2D::Texture2D(uint32_t width, uint32_t height, GLint internalformat, GLenum format, GLenum type,
                     unsigned char *data) {
    setTextureParameters();
    setTextureData(width, height, internalformat, format, type, data);
}

Texture2D::Texture2D(const std::string &fileName) {
    setTextureParameters();
    loadDataFromFile(fileName);
}

void Texture2D::bind() {
    glBindTexture(GL_TEXTURE_2D, getTextureID());
}

void Texture2D::unBind() {
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2D::setTextureParameters() {
    bind();
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    unBind();
}

void Texture2D::generateMipMap() {
    bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);
    unBind();
}

void Texture2D::setTextureData(uint32_t width, uint32_t height, GLint internalformat, GLenum format, GLenum type,
                               unsigned char *data) {
    bind();
    glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0, format, type, data);
    unBind();
}

void Texture2D::loadDataFromFile(const std::string &fileName) {
    bind();
    int width, height, nrChannels;
    unsigned char *data = stbi_load(fileName.c_str(), &width, &height, &nrChannels, 0);
    GLenum format;
    if (nrChannels == 3) format = GL_RGB;
    else if (nrChannels == 4) format = GL_RGBA;

    if (data) {
        setTextureData(width, height, format, format, GL_UNSIGNED_BYTE, data);
    } else {
        std::cout << "Failed to load texture: \n" + fileName << std::endl;
    }
    stbi_image_free(data);
    unBind();
}
