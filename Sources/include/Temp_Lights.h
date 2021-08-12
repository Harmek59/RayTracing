#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include "Buffer.h"


class Temp_Lights
{
public:
	struct light {
		glm::vec3 position;
		float offset1;
		glm::vec3 color;
		float offset2;
		glm::vec3 offset3;
		float radius;

	};


	Temp_Lights(uint32_t numberOfLights);

	void update(double deltaTime);

private:
	Buffer lightsBuffer;
	uint32_t numberOfLights;

	double f = 0.0;
};

