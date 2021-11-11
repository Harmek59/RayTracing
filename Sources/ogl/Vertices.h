#pragma once 
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <initializer_list>

class Vertices{
public:
    Vertices() = default;

    Vertices(float* vertices, size_t verticesSize, const std::initializer_list<unsigned>& attribSizes, 
        unsigned int* indices = nullptr, size_t indicesSize = 0);

    ~Vertices();

    Vertices(Vertices &&);
    Vertices& operator=(Vertices &&);

    //delete copy
    Vertices(const Vertices &) = delete;
    Vertices& operator=(const Vertices &) = delete;

    void create(float* vertices, size_t verticesSize, unsigned int* indices = nullptr, size_t indicesSize = 0);

    void remove();

    void setAttribSizes(const std::initializer_list<unsigned>& attribSizes);

    void bind();

    void unBind();

private:
    GLuint VAO = 0, VBO = 0, EBO = 0;
};