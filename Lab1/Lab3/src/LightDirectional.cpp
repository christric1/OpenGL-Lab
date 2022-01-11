#include "LightDirectional.h"

#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>

LightDirectional::LightDirectional(glm::vec3 _position, glm::vec3 _angles, glm::vec3 _color):
	position(_position),
	angles(_angles),
	color(_color)
{
	updateDiretion();
}

void LightDirectional::updateDiretion() {
	dirction = glm::vec3(0, 0, 1.0f);	// pointing to z (forward)
	dirction = glm::rotateZ(dirction, angles.z);
	dirction = glm::rotateX(dirction, angles.x);
	dirction = glm::rotateY(dirction, angles.y);
	dirction = -1.0f * dirction;
}