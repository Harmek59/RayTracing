#pragma once

#include <glad/glad.h>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <stdexcept>

class Buffer {
public:
    Buffer() = default;

    Buffer(size_t size, GLenum usage, void *data = nullptr);

    ~Buffer();

    Buffer(Buffer &&toMove);

    Buffer &operator=(Buffer &&toMove);

    //delete copy
    Buffer(const Buffer &) = delete;

    Buffer &operator=(const Buffer &) = delete;

    void bind(GLenum bindingTaget = GL_ARRAY_BUFFER);

    void unBind();

    void *mapBuffer(GLenum bufferTarget, GLenum access);

    void unMap(GLenum bufferTarget);

    void bindBufferBase(GLenum target, GLuint bindingIndex);

    template<typename DataType>
    void createShaderStorageBuffer(DataType &data, uint32_t bindingBlock) {
        using Type = typename DataType::value_type;
        (*this) = Buffer(data.size() * sizeof(Type), GL_DYNAMIC_DRAW);
        bind(GL_SHADER_STORAGE_BUFFER);
        Type *dataPtr = (Type *) mapBuffer(GL_SHADER_STORAGE_BUFFER,
                                                GL_WRITE_ONLY | GL_MAP_INVALIDATE_BUFFER_BIT);
        if (!dataPtr) {
            throw std::runtime_error("Buffer createShaderStorageBuffer: mapping buffer failed");
        }
        std::memcpy(dataPtr, data.data(), data.size() * sizeof(Type));

        unMap(GL_SHADER_STORAGE_BUFFER);
        unBind();

        bindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingBlock);
    }

    template<typename DataType>
    void updateShaderStorageBufferData(DataType &data) {
        using Type = typename DataType::value_type;
        bind(GL_SHADER_STORAGE_BUFFER);
        Type *dataPtr = (Type *) mapBuffer(GL_SHADER_STORAGE_BUFFER,
                                           GL_WRITE_ONLY | GL_MAP_INVALIDATE_BUFFER_BIT);
        if (!dataPtr) {
            throw std::runtime_error("Buffer updateShaderStorageBuffer: mapping buffer failed");
        }
        std::memcpy(dataPtr, data.data(), data.size() * sizeof(Type));

        unMap(GL_SHADER_STORAGE_BUFFER);
        unBind();
    }

private:
    void create();

    void allocate(size_t size, GLenum usage, void *data = nullptr);

    void remove();

    GLuint bufferID = 0;
    size_t bufferSize = 0;

    GLenum currentBinding = GL_FALSE; //false mean the buffer is not bind to any target
};