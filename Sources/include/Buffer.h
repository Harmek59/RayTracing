#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>


class Buffer{
public:
    Buffer() = default;
    Buffer(size_t size, GLenum usage, void* data = nullptr);
    ~Buffer();

    Buffer(Buffer&& toMove);
    Buffer& operator=(Buffer&& toMove);
    
    //delete copy
    Buffer(const Buffer &) = delete;
    Buffer& operator=(const Buffer &) = delete;

    void bind(GLenum bindingTaget = GL_ARRAY_BUFFER);
    void unBind();

    void* mapBuffer(GLenum bufferTarget, GLenum access);
    void unMap(GLenum bufferTarget);

    void bindBufferBase(GLenum target, GLuint bindingIndex);
    

    void create();

    void allocate(size_t size, GLenum usage, void* data = nullptr);

    void remove();

//private:
    GLuint bufferID = 0;
    size_t bufferSize = 0;

    GLenum currentBinding = GL_FALSE; //false mean the buffer is not bind to any target

};