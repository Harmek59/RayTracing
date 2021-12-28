#version 330 core

out vec4 fragColor;
flat in vec3 normal;

void main()
{
    vec3 color = vec3(0.7);    // always grey

    // simple lighting
    vec3 directionalLight = vec3(-0.2, -1.0, -0.3);
    float lightIntensity = 0.8;
    color += clamp(dot(normal, directionalLight) * -1, 0.0, 1.0) * lightIntensity;

    fragColor = vec4(color, 1.0);
}
