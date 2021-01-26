#include "Transform.h"

#include <iostream>


Transform::Transform() {
	position = glm::vec3(0.0f);
	orientation = glm::quat(glm::vec3(0.0f));
	scale = glm::vec3(1.0f);
	parent = nullptr;
}

Transform::Transform(glm::vec3 pos, glm::vec3 rot, glm::vec3 scale) {
	position = pos;
	orientation = glm::quat(rot);
	this->scale = scale;
	parent = nullptr;
}

void Transform::SetEulerAngles(glm::vec3 eulerAngles) {
	orientation = glm::normalize(glm::quat(eulerAngles));
}

glm::vec3 Transform::GetEulerAngles() {
	return glm::eulerAngles(orientation);
}

void Transform::Rotate(glm::vec3 eulerAngles) {
	glm::quat rotation = glm::quat(eulerAngles);
	rotation = glm::normalize(rotation);
	orientation = rotation * orientation;
}

void Transform::Rotate(float pitch, float yaw, float roll) {
	glm::quat rotation = glm::quat(glm::vec3(pitch, yaw, roll));
	rotation = glm::normalize(rotation);
	orientation = rotation * orientation;
}

void Transform::Translate(glm::vec3 vector) {
	position += vector;
}

void Transform::Move(glm::vec3 vector) {
	position += orientation * vector;
}

glm::vec3 Transform::GetWorldPosition(glm::vec3 pos) {
	glm::mat4 mat = parent ? parent->GetPositionMatrix() : glm::mat4(1.0f);
	return mat * glm::translate(glm::mat4(1.0f), position) * glm::vec4(pos, 1.0f);
}

glm::mat4 Transform::GetPositionMatrix() {
	glm::mat4 transMatrix = glm::translate(glm::mat4(1.0f), position);
	glm::mat4 rotMatrix = glm::mat4_cast(orientation);

	glm::mat4 parentMatrix = parent ? parent->GetPositionMatrix() : glm::mat4(1.0f);

	return parentMatrix * transMatrix * rotMatrix;
}

glm::mat4 Transform::GetModelMatrix() {
	glm::mat4 scaleMatrix = glm::scale(scale);

	return GetPositionMatrix() * scaleMatrix;
}
