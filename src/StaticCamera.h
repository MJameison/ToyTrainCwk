#pragma once
#include "Camera.h"

#include <GL/glut.h>
#include "Input.h"

class StaticCamera : public Camera {

public:
	StaticCamera() {
		transform = Transform(glm::vec3(0), glm::vec3(0), glm::vec3(1.0f));
	};

	void Update(float deltaTime) override {
		if (Input::IsMousePressed(1)) {
			glm::vec2 offset = Input::GetMouseOffset();
			offset *= sensitivity;
			pitch -= offset.y;
			yaw -= offset.x;
			// Clamp pitch and keep yaw in -180 to 180
			pitch = glm::clamp(pitch, glm::radians(-90.0f), glm::radians(90.0f));

			// Update transform
			transform.orientation = glm::quat(glm::vec3(pitch, yaw, 0.0f));
		}
	};

	glm::mat4 GetViewMatrix() override {
		return glm::inverse(transform.GetPositionMatrix());
	};
	glm::mat4 GetProjectionMatrix() override {
		float viewWidth = (float) glutGet(GLUT_WINDOW_WIDTH);
		float viewHeight = (float) glutGet(GLUT_WINDOW_HEIGHT);
		return glm::perspectiveFov(45.0f, viewWidth, viewHeight, 0.1f, 100.0f);
	};

	float sensitivity = 0.01f;
	float pitch = 0.0f, yaw = 0.0f;
};

