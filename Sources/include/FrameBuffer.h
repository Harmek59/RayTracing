#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Texture2D.h"

class FrameBuffer{
public:
    FrameBuffer();

    ~FrameBuffer();

    FrameBuffer(FrameBuffer &&);
    FrameBuffer& operator=(FrameBuffer &&);

    //delete copy
    FrameBuffer(const FrameBuffer &) = delete;
    FrameBuffer& operator=(const FrameBuffer &) = delete;

    void create();

    void remove();

    GLuint getFrameBufferID();

    void attachTextureToBuffer(Texture2D& texture);

    void bind();

    void unBind();

private:
    GLuint frameBufferID;

};