#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <vector>

#include "Texture2D.h"
#include "TextureCubeMap.h"

class FrameBuffer {
public:
    FrameBuffer();

    ~FrameBuffer();

    FrameBuffer(FrameBuffer &&);

    FrameBuffer &operator=(FrameBuffer &&);

    //delete copy
    FrameBuffer(const FrameBuffer &) = delete;

    FrameBuffer &operator=(const FrameBuffer &) = delete;

    GLuint getFrameBufferID();

    //TODO create Texture base class
    void attachTextureToBuffer(Texture2D &texture, GLenum textureAttachment = GL_COLOR_ATTACHMENT0,
                               std::vector<GLenum> drawBuffers = {GL_COLOR_ATTACHMENT0});

    void attachTextureToBuffer(TextureCubeMap &texture, GLenum textureAttachment = GL_COLOR_ATTACHMENT0,
                               std::vector<GLenum> drawBuffers = {GL_COLOR_ATTACHMENT0});

    void bind();

    void unBind();

private:
    void create();

    void remove();

    GLuint frameBufferID;
};