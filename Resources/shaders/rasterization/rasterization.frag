#version 330 core
out vec4 FragColor;

flat in vec3 Normal;
in vec3 FragPos;

void main()
{
    vec3 color = vec3(0.7);    // always grey

    vec3 directionalLight = vec3(-0.2, -1.0, -0.3);
    float lightIntensity = 0.8;

    color += clamp(dot(Normal, directionalLight) * -1, 0.0, 1.0) * lightIntensity;

    FragColor = vec4(color, 1.0);
}
