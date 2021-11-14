#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <vector>
#include "TextureInterface.h"

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

    void attachTextureToBuffer(TextureInterface &texture, GLenum textureAttachment = GL_COLOR_ATTACHMENT0,
                               std::vector<GLenum> drawBuffers = {GL_COLOR_ATTACHMENT0});

    void bind();

    void unBind();

private:
    void create();

    void remove();

    GLuint frameBufferID = 0;
};