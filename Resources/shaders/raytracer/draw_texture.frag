#version 420 core
out vec4 FragColor;

layout (binding = 0) uniform sampler2D tex_composite;
layout (binding = 1) uniform sampler2D tex_composite2;

uniform int drawFromFirstBinding = 0;

void main()
{
    if (drawFromFirstBinding == 1){
        FragColor = texelFetch(tex_composite2, ivec2(gl_FragCoord.xy * 10), 0);
    } else {
        FragColor = texelFetch(tex_composite, ivec2(gl_FragCoord.xy), 0);
    }
}