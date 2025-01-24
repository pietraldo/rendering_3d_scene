#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Cube
{
private:
	glm::vec3 position;
	glm::vec3 scale;
	glm::vec3 color;
	glm::vec3 rotation;
public:
	Cube(glm::vec3 position, glm::vec3 scale, glm::vec3 color, glm::vec3 rotation)
		:position(position), scale(scale), color(color), rotation(rotation) {};
	glm::vec3 GetPosition() { return position; }
	glm::vec3 GetScale() { return scale; }
	glm::vec3 GetColor() { return color; }
	glm::vec3 GetRotation() { return rotation; }
	glm::mat4 GetModelMatrix();

	static float vertices[216];
};

