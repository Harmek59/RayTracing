#include "FrameBuffer.h"
#include <utility>

FrameBuffer::FrameBuffer(){
    create();
}

FrameBuffer::~FrameBuffer(){
    remove();
}

FrameBuffer::FrameBuffer(FrameBuffer&& toMove){
    remove();
    frameBufferID = std::exchange(toMove.frameBufferID, 0);
}

FrameBuffer& FrameBuffer::operator=(FrameBuffer&& toMove){
    remove();
    frameBufferID = std::exchange(toMove.frameBufferID, 0);
    return *this;
}

void FrameBuffer::create(){
    glGenFramebuffers(1, &frameBufferID);
}

void FrameBuffer::remove(){
    glDeleteFramebuffers(1, &frameBufferID);
    frameBufferID = 0;
}

void FrameBuffer::bind(){
    glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);
}

void FrameBuffer::unBind(){
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint FrameBuffer::getFrameBufferID(){
    return frameBufferID;
}

void FrameBuffer::attachTextureToBuffer(Texture2D& texture){
    bind();
    texture.bind();

    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture.getTextureID(), 0);

    GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, DrawBuffers);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        throw std::runtime_error("Framebuffer not complete!");

    texture.unBind();
    unBind();
}


