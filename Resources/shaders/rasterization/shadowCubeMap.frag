#version 430 core
out vec4 FragColor;

in vec3 TexCoords;

layout(binding = 1) uniform samplerCube skybox;

void main()
{
    FragColor = vec4(vec3(texture(skybox, TexCoords).x), 1.0);
}