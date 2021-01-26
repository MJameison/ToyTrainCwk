#pragma once

#include "GameObject.h"

#include <glm/glm.hpp>

#include "Mesh.h"
#include "Mesh2D.h"

class Track : public GameObject {
public:
	void Init() override;
	void Update(float deltaTime) override;
	void Draw(Shader *shader) override;

	void CreateTrack(std::vector<glm::vec3> controlPoints);

	glm::vec3 GetPosition(float distance);
	glm::quat GetOrientation(float distance);

private:
	Mesh *trackBedMesh;
	Material *trackBedMat;
	Mesh2D extrudeShape;

	bool wireframe = false;
	bool wireframeToggled = false;

	bool culling = true;
	bool cullingToggled = false;

	// Data about the track itself
	std::vector<OrientedPoint> path;
	float totalLength;
	float *distances;
};
