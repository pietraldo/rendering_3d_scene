#pragma once
#include "Light.h"

class LightDirectional : Light
{
private:
	glm::vec3 direction;
public:
	LightDirectional(glm::vec3 position, glm::vec3 color, glm::vec3 direction)
		:Light(position, color), direction(direction) {};
};

