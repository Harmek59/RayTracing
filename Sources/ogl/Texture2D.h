#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>

//TODO: make Texture interface
class Texture2D {
public:
    Texture2D(uint32_t width, uint32_t height, GLint internalformat, GLenum format, GLenum type,
              unsigned char *data = nullptr);

    Texture2D(const std::string &fileName);

    ~Texture2D();

    Texture2D(Texture2D &&);

    Texture2D &operator=(Texture2D &&);

    //delete copy
    Texture2D(const Texture2D &) = delete;

    Texture2D &operator=(const Texture2D &) = delete;

    void setTextureParameters();

    void setTextureData(uint32_t width, uint32_t height, GLint internalformat, GLenum format, GLenum type,
                        unsigned char *data = nullptr);

    void generateMipMap();

    void loadDataFromFile(const std::string &fileName);    //it sets texture data as well

    void bind();

    void unBind();

    GLuint getTextureID();

private:
    void create();

    void remove();

    GLuint textureID = 0;
};