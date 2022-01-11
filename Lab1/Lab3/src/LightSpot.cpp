#include "LightSpot.h"

LightSpot::LightSpot(glm::vec3 _position, glm::vec3 _angles, glm::vec3 _color):
	position(_position),
	angles(_angles),
	color(_color)
{	
	constant = 1.0f;
	linear = 0.09f;
	quadratic = 0.032f;

	UpdateDirection();
}

void LightSpot::UpdateDirection()
{
	dirction = glm::vec3(0, 0, 1.0f);	// pointing to z (forward)
	dirction = glm::rotateZ(dirction, angles.z);
	dirction = glm::rotateX(dirction, angles.x);
	dirction = glm::rotateY(dirction, angles.y);
	dirction = -1.0f * dirction;
}
