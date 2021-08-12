#pragma once 
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <iostream>
#include <cstdint>

class Texture2D{
public:
    Texture2D(uint32_t width, uint32_t height, GLint internalformat, GLenum format, GLenum type, unsigned char *data = nullptr);

    Texture2D(const std::string& fileName);

    ~Texture2D();

    Texture2D(Texture2D &&);
    Texture2D& operator=(Texture2D &&);

    //delete copy
    Texture2D(const Texture2D &) = delete;
    Texture2D& operator=(const Texture2D &) = delete;

    void create();

    void setTextureParameters();

    void setTextureData(uint32_t width, uint32_t height, GLint internalformat, GLenum format, GLenum type, unsigned char *data = nullptr);

    void generateMipMap();

    void loadDataFromFile(const std::string& fileName);    //it sets data as well

    void bind();

    void unBind();

    GLuint getTextureID();

    void remove();       

private:
    GLuint textureID = 0;
};