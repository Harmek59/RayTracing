#include "Temp_Lights.h"
#include <stdexcept>


Temp_Lights::Temp_Lights(uint32_t numberOfLights) : lightsBuffer{ numberOfLights * sizeof(light), GL_DYNAMIC_DRAW }
{
    this->numberOfLights = numberOfLights;
}

void Temp_Lights::update(double deltaTime)
{

    f += deltaTime * 0.1;

    uint32_t uniformBlockBinding = 3;
    lightsBuffer.bind(GL_UNIFORM_BUFFER);
    light* l = (light*)lightsBuffer.mapBuffer(GL_UNIFORM_BUFFER, GL_READ_WRITE | GL_MAP_INVALIDATE_BUFFER_BIT);
    if (!l) {
        throw std::runtime_error("Temp_Lights error during update function: mapping buffer failed");
    }
    for (int i = 0; i < int(numberOfLights); i++)
    {

        float fi = (float)i / 128.0f;
        l[i].position = glm::vec3(sinf(fi * 123.0f + f) * 50.75f,  (cosf(fi * 456.0f + f) +1.0) * 15.75f, (sinf(fi * 300.0f + f) * cosf(fi * 200.0f + f)) * 50.0f);
        //l[i].position = glm::vec3(-20.0, 20.0, -20.0);
        l[i].color = glm::vec3(1.0);

        l[i].radius = 300.;
    }

    lightsBuffer.unMap(GL_UNIFORM_BUFFER);
    lightsBuffer.unBind();

    lightsBuffer.bindBufferBase(GL_UNIFORM_BUFFER, uniformBlockBinding);

}
