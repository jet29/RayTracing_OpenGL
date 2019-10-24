#include "Camera.h"

const float MOVEMENT_SPEED = 20.0f;


Camera::Camera() :
	viewDirection(0, 0, -1),
	UP(0.0f, 1.0f, 0.0f),
	position(0, 0, 1.281f)
{
	yaw = 0.0f;
	pitch = 0.0f;
}

void Camera::mouseUpdate(const glm::vec2 &newMousePosition) {
	yaw += newMousePosition.x;
	pitch += newMousePosition.y;
	glm::mat4 Rotation = glm::yawPitchRoll(glm::radians(yaw), glm::radians(pitch), 0.0f);
	viewDirection = glm::vec3(Rotation * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f));
	UP = glm::vec3(Rotation * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));
}

glm::mat4 Camera::getWorldToViewMatrix() const {
	//printf("position (%f,%f,%f) viewDiw (%f,%f,%f)\n", position[0], position[1], position[2], viewDirection[0], viewDirection[1], viewDirection[2]);
	return glm::lookAt(position, position + viewDirection, UP);
}

bool isAvalidPosition(glm::vec3 position) {
	return position.x >= -9.0f  && position.x <= 9.0f &&
		   position.y >= -6.0f  && position.y <= 6.0f &&
		   position.z >= -29.0f && position.z <= 2.0f   
	? true : false;
}

void Camera::moveForward(float time) {
	float speed = MOVEMENT_SPEED * time;
	glm::vec3 newPos = position + speed * viewDirection;
	if (isAvalidPosition(newPos))
		position = newPos;
}

void Camera::moveBackward(float time) {
	float speed = MOVEMENT_SPEED * time;
	glm::vec3 newPos = position - speed * viewDirection;
	if (isAvalidPosition(newPos))
		position = newPos;
}

void Camera::moveLeft(float time) {
	float speed = MOVEMENT_SPEED * time;
	glm::vec3 moveDirection = glm::cross(viewDirection, UP);
	glm::vec3 newPos = position + (-speed) * moveDirection;
	if (isAvalidPosition(newPos))
		position = newPos;
}

void Camera::moveRight(float time) {
	float speed = MOVEMENT_SPEED * time;
	glm::vec3 moveDirection = glm::cross(viewDirection, UP);
	glm::vec3 newPos = position + speed * moveDirection;
	if (isAvalidPosition(newPos))
		position = newPos;
}

void Camera::moveUp(float time) {
	float speed = MOVEMENT_SPEED * time;
	glm::vec3 newPos = position + speed * UP;
	if (isAvalidPosition(newPos))
		position = newPos;
}

void Camera::moveDown(float time) {
	float speed = MOVEMENT_SPEED * time;
	glm::vec3 newPos = position - speed * UP;
	if (isAvalidPosition(newPos))
		position = newPos;
}