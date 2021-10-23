#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

#include "Texture2D.h"
#include "TextureCubeMap.h"

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

    void attachTextureToBuffer(Texture2D& texture, GLenum textureAttachment = GL_COLOR_ATTACHMENT0, std::vector<GLenum> drawBuffers = {GL_COLOR_ATTACHMENT0});  //TODO create Texture base class
    void attachTextureToBuffer(TextureCubeMap& texture, GLenum textureAttachment = GL_COLOR_ATTACHMENT0, std::vector<GLenum> drawBuffers = {GL_COLOR_ATTACHMENT0});


    void bind();

    void unBind();

private:
    GLuint frameBufferID;

};