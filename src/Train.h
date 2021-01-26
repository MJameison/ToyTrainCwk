#pragma once

#include "GameObject.h"
#include "Camera.h"
#include "Track.h"

class Train : public GameObject {
public:
	void Init() override;
	void Update(float deltaTime) override;

private:
	Camera *trainCamera;
	Camera *freeCamera;
	Track *track;

	GameObject *modelObj;

	float distance;
	float speed;
	float stopping;
	float pressedSpace; // To prevent repeat events

	std::vector<glm::vec3> controlPoints;

	void loadControlPoints();
};
