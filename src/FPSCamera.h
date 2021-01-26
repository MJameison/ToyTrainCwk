#pragma once

#include "Input.h"
#include "Camera.h"

class FPSCamera : public virtual Camera {
public:
	FPSCamera(float sensitivity);
	void Update(float deltaTime) override;
	glm::mat4 GetViewMatrix() override;
	glm::mat4 GetProjectionMatrix() override;

	float cameraSpeed;
	float sensitivity;
private:
	float pitch;
	float yaw;
};

