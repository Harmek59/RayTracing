#include "Buffer.h"
#include <utility>
#include <iostream>

Buffer::Buffer(size_t size, GLenum usage, void* data){
    create();
    allocate(size, usage, data);
}

Buffer::~Buffer(){
    remove();
}

Buffer::Buffer(Buffer&& toMove){
    remove();
    bufferID = std::exchange(toMove.bufferID, 0);
}

Buffer& Buffer::operator=(Buffer&& toMove){
    remove();
    bufferID = std::exchange(toMove.bufferID, 0);
    return *this;
}


void Buffer::create(){
    glGenBuffers(1, &bufferID);
}

void Buffer::remove(){
    glDeleteBuffers(1, &bufferID);
    bufferID = 0;
}

void Buffer::allocate(size_t size, GLenum usage, void* data){
    bind();
    glBufferData(GL_ARRAY_BUFFER, size, data, usage);
    bufferSize = size;
    unBind();
}

void Buffer::bind(GLenum bindingTaget){
    if(currentBinding != GL_FALSE)
        unBind();
    glBindBuffer(bindingTaget, bufferID);
    currentBinding = bindingTaget;
}

void Buffer::unBind(){
    if(currentBinding != GL_FALSE){
        glBindBuffer(currentBinding, 0);
        currentBinding = GL_FALSE;
    }
}

void* Buffer::mapBuffer(GLenum bufferTarget, GLenum access){
    void* ptr = glMapBuffer(bufferTarget, access);
    return ptr;
}

void Buffer::unMap(GLenum bufferTarget){
    glUnmapBuffer(bufferTarget);
}

void Buffer::bindBufferBase(GLenum target, GLuint bindingIndex){
    glBindBufferBase(target, bindingIndex, bufferID);
}

