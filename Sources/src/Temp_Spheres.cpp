#include "Temp_Spheres.h"
#include <exception>


Temp_Spheres::Temp_Spheres(uint32_t numberOfSpheres): spheresBuffer{numberOfSpheres * sizeof(sphere), GL_DYNAMIC_DRAW}
{
	this->numberOfSpheres = numberOfSpheres;
}

void Temp_Spheres::update(double deltaTime)
{

    f += deltaTime * 0.1;

    uint32_t uniformBlockBinding = 1;
    //861
    spheresBuffer.bind(GL_UNIFORM_BUFFER);
    sphere* s = (sphere*)spheresBuffer.mapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY | GL_MAP_INVALIDATE_BUFFER_BIT);
    if (!s) {
        throw std::exception("Temp_Spheres error during update function: mapping buffer failed");
    }
    for (int i = 0; i < numberOfSpheres; i++)
    {

        float fi = (float)i / 128.0f;
        s[i].center = glm::vec3(sinf(fi * 123.0f + f) * 15.75f, cosf(fi * 456.0f + f) * 15.75f, (sinf(fi * 300.0f + f) * cosf(fi * 200.0f + f)) * 20.0f);
        s[i].radius = fi * 2.3f + 3.5f;
        float r = fi * 61.0f;
        float g = r + 0.25f;
        float b = g + 0.25f;
        r = (r - std::floor(r)) * 0.8f + 0.2f;
        g = (g - std::floor(g)) * 0.8f + 0.2f;
        b = (b - std::floor(b)) * 0.8f + 0.2f;
        s[i].color = glm::vec4(r, g, b, 1.0f);
        s[i].energy = glm::vec4(0.7);
    }

    spheresBuffer.unMap(GL_UNIFORM_BUFFER);
    spheresBuffer.unBind();

    spheresBuffer.bindBufferBase(GL_UNIFORM_BUFFER, uniformBlockBinding);

}
