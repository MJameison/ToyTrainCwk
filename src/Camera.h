#pragma once

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "Transform.h"

class Camera {
public:
	virtual void Update(float deltaTime) {};
	virtual glm::mat4 GetViewMatrix() { return glm::mat4(1.0f); };
	virtual glm::mat4 GetProjectionMatrix() { return glm::mat4(1.0f); };
	Transform transform;

protected:
	Camera() {};
};

