#pragma once

#include "Light.h"
#include <glm/glm.hpp>

class LightSpot : Light
{
private:
	float constant;
	float linear;
	float quadratic;
	float cutOff;
	float outerCutOff;
	glm::vec3 direction;
public:
	LightSpot(glm::vec3 position, glm::vec3 color, float constant, float linear, float quadratic, float cutOff, float outerCutOff, glm::vec3 direction)
		:Light(position, color), constant(constant), linear(linear), quadratic(quadratic), cutOff(cutOff), outerCutOff(outerCutOff), direction(direction) {};
};

