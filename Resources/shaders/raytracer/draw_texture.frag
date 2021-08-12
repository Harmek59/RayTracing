#version 420 core
out vec4 FragColor;

layout (binding = 0) uniform sampler2D tex_composite;


void main()
{
	FragColor = texelFetch(tex_composite, ivec2(gl_FragCoord.xy), 0);
}