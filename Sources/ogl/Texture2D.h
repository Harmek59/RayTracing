#pragma once

#include "TextureInterface.h"
#include <glm/glm.hpp>

#include <string>

class Texture2D : public TextureInterface {
public:
    Texture2D();

    Texture2D(uint32_t width, uint32_t height, GLint internalformat, GLenum format, GLenum type,
              unsigned char *data = nullptr);

    Texture2D(const std::string &fileName);

    void setTextureData(uint32_t width, uint32_t height, GLint internalformat, GLenum format, GLenum type,
                        unsigned char *data = nullptr);

    void generateMipMap();

    void loadDataFromFile(const std::string &fileName);    //it sets texture data as well

    virtual void bind() override;

    virtual void unBind() override;

private:
    virtual void setTextureParameters() override;
};