#pragma once

#include <glad/glad.h>

class TextureInterface {
public:
    TextureInterface();

    virtual ~TextureInterface();

    TextureInterface(TextureInterface &&);

    TextureInterface &operator=(TextureInterface &&);

    //delete copy
    TextureInterface(const TextureInterface &) = delete;

    TextureInterface &operator=(const TextureInterface &) = delete;

    virtual void bind() = 0;

    virtual void unBind() = 0;

    GLuint getTextureID();

private:
    void create();

    void remove();

    virtual void setTextureParameters() = 0;

    GLuint textureID = 0;
};