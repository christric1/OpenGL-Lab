#include "Camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 target, glm::vec3 worldup) {

	CamaraPos = position;
	CamaraTarget = target;
	Worldup = worldup;
	CamaraForward = glm::normalize(CamaraPos - target);
	CamaraRight = glm::normalize(glm::cross(Worldup, CamaraForward));
	CamaraUp = glm::normalize(cross(CamaraForward, CamaraRight));

}

Camera::Camera(glm::vec3 position, float pitch, float yaw, glm::vec3 worldup){
	CamaraPos = position;
	Worldup = worldup;
	Pitch = pitch;
	Yaw = yaw;
	CamaraForward.x = cos(glm::radians(Pitch)) * cos(glm::radians(Yaw));
	CamaraForward.y = sin(glm::radians(Pitch));
	CamaraForward.z = cos(glm::radians(Pitch)) * sin(glm::radians(Yaw));
	CamaraForward = glm::normalize(CamaraForward);
	CamaraRight = glm::normalize(glm::cross(CamaraForward, Worldup));
	CamaraUp = glm::normalize(cross(CamaraRight, CamaraForward));
}

glm::mat4 Camera::GetViewMatrix() {
	return glm::lookAt(CamaraPos, CamaraPos + CamaraForward, dick);
}

void Camera::ProcessMouseMovement(float DeltaX, float DeltaY){

	Pitch += DeltaY;
	Yaw += DeltaX;

	if (Pitch > 89.0f)
		Pitch = 89.0f;
	if (Pitch < -89.0f)
		Pitch = -89.0f;

	UpdataCameraVectors();
}

void Camera::UpdataCameraVectors(){
	CamaraForward.x = cos(glm::radians(Pitch)) * cos(glm::radians(Yaw));
	CamaraForward.y = sin(glm::radians(Pitch));
	CamaraForward.z = cos(glm::radians(Pitch)) * sin(glm::radians(Yaw));
	CamaraForward = glm::normalize(CamaraForward);
	CamaraRight = glm::normalize(glm::cross(CamaraForward, Worldup));
	CamaraUp = glm::normalize(cross(CamaraRight, CamaraForward));
}