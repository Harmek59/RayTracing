#version 430 core
out vec4 FragColor;

in vec3 TexCoords;

layout(binding = 2) uniform samplerCube cubeMap;

void main()
{
    FragColor = texture(cubeMap, TexCoords);
}