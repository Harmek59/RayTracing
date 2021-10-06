#version 330 core
out vec4 FragColor;

flat in vec3 Normal;
in vec3 FragPos;
flat in uint materialID;



struct Material{
    vec3 albedo;
    vec3 specular;
};

Material materials[5];

void main()
{
    materials[0].specular = vec3(0.2);
    materials[0].albedo = vec3(0.7);

    materials[1].specular = vec3(0.1);
    materials[1].albedo = vec3(1.0, 0.0, 0.0);
    materials[2].specular = vec3(0.1);
    materials[2].albedo = vec3(0.0, 1.0, 0.0);
    materials[3].specular = vec3(0.9);
    materials[3].albedo = vec3(0.0, 0.0, 1.0);
    materials[4].specular = vec3(0.1);
    materials[4].albedo = vec3(1.0, 0.0, 0.1);

    vec3 color = materials[materialID].albedo;
    color = materials[0].albedo;    // always grey

    vec3 directionalLight = vec3(-0.2, -1.0, -0.3);
    float lightIntensity = 0.8;

    color += clamp(dot(Normal, directionalLight) * -1, 0.0, 1.0) * lightIntensity;

    FragColor = vec4(color, 1.0);
}
