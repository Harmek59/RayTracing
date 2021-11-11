#pragma once

#include "TextureInterface.h"

#include <array>
#include <string>

class TextureCubeMap : public TextureInterface {
public:
    TextureCubeMap(uint32_t width, uint32_t height, GLint internalformat, GLenum format, GLenum type,
                   const std::array<uint8_t *, 6> &data = {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr});

    TextureCubeMap(const std::array<std::string, 6> &fileNameArr);


    void setTextureData(uint32_t width, uint32_t height, GLint internalformat, GLenum format, GLenum type,
                        const std::array<uint8_t *, 6> &data);

    void loadDataFromFile(const std::array<std::string, 6> &fileNameArr);

    virtual void bind() override;

    virtual void unBind() override;

private:
    virtual void setTextureParameters() override;
};