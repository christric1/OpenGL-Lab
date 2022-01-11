#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>

class LightDirectional
{
public:
	glm::vec3 position;
	glm::vec3 angles;
	glm::vec3 dirction;
	glm::vec3 color;

	LightDirectional(glm::vec3 _position, glm::vec3 _angles, glm::vec3 _color=glm::vec3(1.0f, 1.0f, 1.0f));

	void updateDiretion();

};

