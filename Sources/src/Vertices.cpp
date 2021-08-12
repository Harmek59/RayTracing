#include "Vertices.h"
#include <iostream>
#include <numeric>
#include <utility>

Vertices::Vertices(float* vertices, size_t verticesSize, 
    const std::initializer_list<unsigned>& attribSizes, unsigned int* indices, size_t indicesSize){
    
    create(vertices, verticesSize, indices, indicesSize);
    setAttribSizes(attribSizes);
}

Vertices::~Vertices(){
    remove();
}

Vertices::Vertices(Vertices&& toMove){
    remove();
    VAO = std::exchange(toMove.VAO, 0);
    VBO = std::exchange(toMove.VBO, 0);
    EBO = std::exchange(toMove.EBO, 0);
}
Vertices& Vertices::operator=(Vertices&& toMove){
    remove();
    VAO = std::exchange(toMove.VAO, 0);
    VBO = std::exchange(toMove.VBO, 0);
    EBO = std::exchange(toMove.EBO, 0);
    return *this;
}


void Vertices::create(float* vertices, size_t verticesSize, unsigned int* indices, size_t indicesSize){
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    if(indicesSize)
        glGenBuffers(1, &EBO);

    bind();
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, verticesSize * sizeof(float), vertices, GL_STATIC_DRAW);

    if(indicesSize){
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize * sizeof(unsigned), indices, GL_STATIC_DRAW);
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, 0); 

    unBind();
}

void Vertices::remove(){
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    VAO = 0;
    VBO = 0;
    EBO = 0;
}

void Vertices::setAttribSizes(const std::initializer_list<unsigned>& attribSizes){
    bind();
    
    unsigned offsetOfAllAttribiutes = std::accumulate(attribSizes.begin(), attribSizes.end(), 0);

    glBindBuffer(GL_ARRAY_BUFFER, VBO); 

    unsigned beginOfAttribute = 0;
    unsigned positionOfAttribute = 0;
    for(unsigned attribSize: attribSizes){
        glVertexAttribPointer(positionOfAttribute, attribSize, GL_FLOAT, GL_FALSE, offsetOfAllAttribiutes * sizeof(float), 
            (void*)(beginOfAttribute * sizeof(float)));
        glEnableVertexAttribArray(positionOfAttribute);
        beginOfAttribute += attribSize;
        positionOfAttribute++;
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    unBind();
}

void Vertices::bind(){
    glBindVertexArray(VAO);
}

void Vertices::unBind(){
    glBindVertexArray(0);
}