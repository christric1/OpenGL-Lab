#pragma once

#ifndef  _Camera_h_
#define _Camera_h_

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera
{
public:
	Camera(glm::vec3 position, glm::vec3 target, glm::vec3 worldup);
	Camera(glm::vec3 position, float pitch, float yaw, glm::vec3 worldup);

	glm::vec3 CamaraPos = glm::vec3(0.0f, 0.0f, 3.0f);	        // 相機位置
	glm::vec3 CamaraTarget = glm::vec3(0.0f, 0.0f, 0.0f);	    // 目標位置
	glm::vec3 CamaraForward = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 CamaraRight;
	glm::vec3 CamaraUp = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 Worldup = glm::vec3(0.0f, 1.0f, 0.0f);	// 世界往上向量
	glm::vec3 dick = glm::vec3(0.0f, 1.0f, 0.0f);

	float Pitch = 0.0f;
	float Yaw = -90.0f;

	glm::mat4 GetViewMatrix();
	void ProcessMouseMovement(float DeltaX, float DeltaY);

private:

	void UpdataCameraVectors();
};

#endif  

