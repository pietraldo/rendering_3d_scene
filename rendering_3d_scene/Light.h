#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


class Light
{
protected:
	glm::vec3 position;
	glm::vec3 color;
public:
	Light(glm::vec3 position, glm::vec3 color)
		:position(position), color(color) {};
	glm::vec3 GetPosition() { return position; }
	glm::vec3 GetColor() { return color; }

};

