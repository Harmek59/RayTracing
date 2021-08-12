#version 430 core

uniform int imageSizeX;
uniform int imageSizeY;

out ivec2 imgCoord;

void main()
{
    imgCoord = ivec2(gl_VertexID% imageSizeX, gl_VertexID/imageSizeX);
	gl_Position = vec4(0.0);
}