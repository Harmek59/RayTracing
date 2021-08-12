#include "GLSLShader.h"

#include <iostream>
#include <stdexcept>
#include <utility>

GLSLShader::GLSLShader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath, const std::string& geometryShaderPath):
    GLSLShader(vertexShaderPath, fragmentShaderPath, "", "", geometryShaderPath){
}

GLSLShader::GLSLShader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath, const std::string& tessControlShaderPath, 
    const std::string& tessEvaluationShaderPath, const std::string& geometryShaderPath){
    createProgram();

    GLuint shader = createAndCompileShader(readShaderFromFile(vertexShaderPath), GL_VERTEX_SHADER, "Vertex Shader: " + vertexShaderPath);
    attachShaderToProgram(shader, GL_VERTEX_SHADER);

    shader = createAndCompileShader(readShaderFromFile(fragmentShaderPath), GL_FRAGMENT_SHADER, "Fragment Shader: " + fragmentShaderPath);
    attachShaderToProgram(shader, GL_FRAGMENT_SHADER);

    if(tessControlShaderPath != ""){
        shader = createAndCompileShader(readShaderFromFile(tessControlShaderPath), GL_TESS_CONTROL_SHADER, "Tesselation control Shader: " + tessControlShaderPath);
        attachShaderToProgram(shader, GL_TESS_CONTROL_SHADER);
    }
    if(tessEvaluationShaderPath != ""){
        shader = createAndCompileShader(readShaderFromFile(tessEvaluationShaderPath), GL_TESS_EVALUATION_SHADER, "Tesselation evaluation Shader: " + tessEvaluationShaderPath);
        attachShaderToProgram(shader, GL_TESS_EVALUATION_SHADER);
    }
    if(geometryShaderPath != ""){
        shader = createAndCompileShader(readShaderFromFile(geometryShaderPath), GL_GEOMETRY_SHADER, "Geometry Shader: " + geometryShaderPath);
        attachShaderToProgram(shader, GL_GEOMETRY_SHADER);
    }
    
    linkProgram();
}

GLSLShader::GLSLShader(const std::string& computeShaderPath){
    createProgram();

    GLuint shader = createAndCompileShader(readShaderFromFile(computeShaderPath), GL_COMPUTE_SHADER, "Compute Shader: " + computeShaderPath);
    attachShaderToProgram(shader, GL_COMPUTE_SHADER);

    linkProgram();
}

void GLSLShader::remove(){
    glDeleteProgram(programID);
    programID = 0;
}

GLSLShader::~GLSLShader(){
    remove();
}

GLSLShader::GLSLShader(GLSLShader&& toMove){
    remove();
    programID = std::exchange(toMove.programID, 0);
}
GLSLShader& GLSLShader::operator=(GLSLShader&& toMove){
    remove();
    programID = std::exchange(toMove.programID, 0);
    return *this;
}

void GLSLShader::use() const{
    glUseProgram(programID); 
}

void GLSLShader::unUse() const{
    glUseProgram(0);
}

std::string GLSLShader::readShaderFromFile(const std::string& shaderPath){
    std::ifstream shaderFile;
    shaderFile.open(shaderPath, std::ifstream::in);

    if(!shaderFile.good()){
        std::cout<<"ERROR: Failed to read shader from file:\n" + shaderPath <<std::endl;
    }
    
    std::string shaderCode;
    std::string line;
    while(getline(shaderFile, line)){
        shaderCode += line + '\n';
    }

    shaderFile.close();
    return shaderCode;
}

GLuint GLSLShader::createAndCompileShader(const std::string& shaderCode, GLenum shaderType, const std::string& shaderName){
    GLuint shader = glCreateShader(shaderType);
    const char* cString = shaderCode.c_str();
    glShaderSource(shader, 1, &cString, NULL);
    glCompileShader(shader);

    GLint success;
    GLchar infoLog[1024];

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(shader, 1024, NULL, infoLog);
        std::cout<<"Failed to compile shader:\n" + shaderName + '\n' + infoLog <<std::endl;
    }

    return shader;
}

void GLSLShader::createProgram(){
    programID = glCreateProgram();
}

void GLSLShader::attachShaderToProgram(GLuint shaderID, GLenum shaderType){
    glAttachShader(programID ,shaderID);
    glDeleteShader(shaderID);
}

void GLSLShader::linkProgram(){
    glLinkProgram(programID);

    GLint success;
    GLchar infoLog[1024];

    glGetProgramiv(programID, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(programID, 1024, NULL, infoLog);
        std::cout<<"Failed to link program:\n" + std::string(infoLog) <<std::endl;
    }
}

void GLSLShader::setBool(const std::string &name, bool value) const
{         
    glUniform1i(glGetUniformLocation(programID, name.c_str()), (int)value); 
}

void GLSLShader::setInt(const std::string &name, int value) const
{ 
    glUniform1i(glGetUniformLocation(programID, name.c_str()), value); 
}

void GLSLShader::setFloat(const std::string &name, float value) const
{ 
    glUniform1f(glGetUniformLocation(programID, name.c_str()), value); 
}
   
void GLSLShader::setVec2(const std::string &name, const glm::vec2 &value) const
{ 
    glUniform2fv(glGetUniformLocation(programID, name.c_str()), 1, &value[0]); 
}
void GLSLShader::setVec2(const std::string &name, float x, float y) const
{ 
    glUniform2f(glGetUniformLocation(programID, name.c_str()), x, y); 
}
    
void GLSLShader::setVec3(const std::string &name, const glm::vec3 &value) const
{ 
    glUniform3fv(glGetUniformLocation(programID, name.c_str()), 1, &value[0]); 
}
void GLSLShader::setVec3(const std::string &name, float x, float y, float z) const
{ 
    glUniform3f(glGetUniformLocation(programID, name.c_str()), x, y, z); 
}

void GLSLShader::setVec4(const std::string &name, const glm::vec4 &value) const
{ 
    glUniform4fv(glGetUniformLocation(programID, name.c_str()), 1, &value[0]); 
}
void GLSLShader::setVec4(const std::string &name, float x, float y, float z, float w) const
{ 
    glUniform4f(glGetUniformLocation(programID, name.c_str()), x, y, z, w); 
}

void GLSLShader::setMat2(const std::string &name, const glm::mat2 &mat) const
{
    glUniformMatrix2fv(glGetUniformLocation(programID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void GLSLShader::setMat3(const std::string &name, const glm::mat3 &mat) const
{
    glUniformMatrix3fv(glGetUniformLocation(programID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
   
void GLSLShader::setMat4(const std::string &name, const glm::mat4 &mat) const
{
    glUniformMatrix4fv(glGetUniformLocation(programID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}


