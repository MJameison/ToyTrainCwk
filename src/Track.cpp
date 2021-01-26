#include "Track.h"

#include <iostream>
#include <vector>
#include "ResourceManager.h"
#include "Input.h"

#include "CatmullRom.h"

void Track::Init() {
	trackBedMat = new Material();
	trackBedMat->SetDiffuseColor(glm::vec3(1.0f));
	trackBedMat->SetSpecularColor(glm::vec3(0.8f));
	trackBedMat->SetSpecularStrength(1.0f);
	trackBedMat->SetDiffuseTexture(ResourceManager::LoadTexture("resources/track_diffuse.png"));
	trackBedMat->SetSpecularTexture(ResourceManager::LoadTexture("resources/track_specular.png"));
	trackBedMat->SetMapping(TextureMapping::REPEAT, TextureMapping::REPEAT);
	trackBedMat->SetShininess(128.0f);

	// Setup extrudeMesh
	extrudeShape.AddVertex({ glm::vec2(-0.6, 0.0), glm::vec2(-0.1, 0.38), 0.0f });				// Start of ramp up
	extrudeShape.AddVertex({ glm::vec2(-0.22, 0.1), glm::vec2(-0.1, 0.38), 282.0f / 1024.0f });	// End of ramp up
	extrudeShape.AddVertex({ glm::vec2(-0.22, 0.1), glm::vec2(-1.0, 0.0), 282.0f / 1024.0f });	// Bottom of rise 1
	extrudeShape.AddVertex({ glm::vec2(-0.22, 0.15), glm::vec2(-1.0, 0.0), 333.0f / 1024.0f });	// Top of rise 1
	extrudeShape.AddVertex({ glm::vec2(-0.22, 0.15), glm::vec2(0.0, 1.0), 333.0f / 1024.0f });	// Left of rise 1
	extrudeShape.AddVertex({ glm::vec2(-0.18, 0.15), glm::vec2(0.0, 1.0), 374.0f / 1024.0f });	// Right of rise 1
	extrudeShape.AddVertex({ glm::vec2(-0.18, 0.15), glm::vec2(1.0, 0.0), 374.0f / 1024.0f });	// Top of fall 1
	extrudeShape.AddVertex({ glm::vec2(-0.18, 0.1), glm::vec2(1.0, 0.0), 425.0f / 1024.0f });	// Bottom of fall 1
	extrudeShape.AddVertex({ glm::vec2(-0.18, 0.1), glm::vec2(0.0, 1.0), 425.0f / 1024.0f });	// Flat middle start
	extrudeShape.AddVertex({ glm::vec2(0.18, 0.1), glm::vec2(0.0, 1.0), 599.0f / 1024.0f });	// Flat middle end
	extrudeShape.AddVertex({ glm::vec2(0.18, 0.1), glm::vec2(-1.0, 0.0), 599.0f / 1024.0f });	// Bottom of rise 2
	extrudeShape.AddVertex({ glm::vec2(0.18, 0.15), glm::vec2(-1.0, 0.0), 650.0f / 1024.0f });	// Top of rise 2
	extrudeShape.AddVertex({ glm::vec2(0.18, 0.15), glm::vec2(0.0, 1.0), 650.0f / 1024.0f });	// Left of rise 2
	extrudeShape.AddVertex({ glm::vec2(0.22, 0.15), glm::vec2(0.0, 1.0), 691.0f / 1024.0f });	// Right of rise 2
	extrudeShape.AddVertex({ glm::vec2(0.22, 0.15), glm::vec2(1.0, 0.0), 691.0f / 1024.0f });	// Top of fall 2
	extrudeShape.AddVertex({ glm::vec2(0.22, 0.1), glm::vec2(1.0, 0.0), 745.0f / 1024.0f });	// Bottom of fall 2
	extrudeShape.AddVertex({ glm::vec2(0.22, 0.1), glm::vec2(0.1, 0.38), 745.0f / 1024.0f });	// Start of ramp down
	extrudeShape.AddVertex({ glm::vec2(0.6, 0.0), glm::vec2(0.1, 0.38), 1.0f });				// End of ramp down
	extrudeShape.AddLine(0, 1);
	extrudeShape.AddLine(2, 3);
	extrudeShape.AddLine(4, 5);
	extrudeShape.AddLine(6, 7);
	extrudeShape.AddLine(8, 9);
	extrudeShape.AddLine(10, 11);
	extrudeShape.AddLine(12, 13);
	extrudeShape.AddLine(14, 15);
	extrudeShape.AddLine(16, 17);

	// We will generate the track mesh here
	model = new Model();

}

void Track::Update(float deltaTime) {
	// Change culling
	if (!cullingToggled && Input::IsKeyPressed('c')) {
		culling = !culling;
		cullingToggled = true;
	}
	if (cullingToggled && !Input::IsKeyPressed('c')) {
		cullingToggled = false;
	}

	// Change wireframe
	if (!wireframeToggled && Input::IsKeyPressed('v')) {
		wireframeToggled = true;
		wireframe = !wireframe;
		// Remove texture to render in white instead
		//trackBedMat->SetDiffuseTexture(wireframe ? nullptr : ResourceManager::LoadTexture("resources/track_diffuse.png"));
		trackBedMat->SetDiffuseColor(glm::vec3(1.0f) * (wireframe ? 0.0f : 1.0f));
	}
	if (wireframeToggled && !Input::IsKeyPressed('v')) {
		wireframeToggled = false;
	}
}

void Track::Draw(Shader *shader) {
	if (!culling)
		glDisable(GL_CULL_FACE);
	if (wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// Use default draw now we've wrapped it with the gl calls
	GameObject::Draw(shader);

	// return to defaults
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_CULL_FACE);
}

void Track::CreateTrack(std::vector<glm::vec3> controlPoints) {
	path.clear();
	Model *oldModel = this->model;
	this->model = new Model();

	std::vector<CatmullRom::CatmullRomPoint> catmullPoints = CatmullRom::EvaluateSpline(controlPoints, true);

	for (auto &cPoint : catmullPoints) {
		OrientedPoint p;
		p.position = cPoint.position;
		p.orientation = glm::quatLookAtRH(cPoint.tangent, cPoint.normal);
		path.push_back(p);
	}

	// Calculate distances
	unsigned int count = path.size();
	delete[] distances;
	distances = new float[count];

	totalLength = 0.0f;
	distances[0] = 0.0f;
	for (unsigned int i = 1; i < path.size(); ++i) {
		totalLength += glm::distance(path[i - 1].position, path[i].position);
		distances[i] = totalLength;
	}
	// Add the loop back
	totalLength += glm::distance(path[count - 1].position, path[0].position);

	trackBedMesh = extrudeShape.Extrude(path, distances);
	// Add meshes to models
	model->AddMesh(trackBedMesh, trackBedMat);
}


glm::vec3 Track::GetPosition(float distance) {
	float dist = glm::mod(distance, totalLength); // The amount round in [0..totalLength)
	// Get i and j the index of the point before and after dist distance
	int i;
	for (i = 0; i < path.size(); ++i) {
		if (dist < distances[i])
			break;
	}
	int j = (i + 1) % path.size();

	// Get the points corresponding to i and j
	OrientedPoint p0 = path[i];
	OrientedPoint p1 = path[j];
	
	// Calculate the t value for distances
	float dist0 = distances[i];
	float dist1 = j == 0 ? totalLength : distances[j]; // Give total Length if after the lst point (since its a loop)
	float t = (dist - dist0) / (dist1 - dist0);

	// Interpolate the position
	return (1-t) * p0.position + t * p1.position;
}

glm::quat Track::GetOrientation(float distance) {
	float dist = glm::mod(distance, totalLength); // The amount round in [0..totalLength)
	// Get i and j the index of the point before and after dist distance
	int i;
	for (i = 0; i < path.size(); ++i) {
		if (dist < distances[i])
			break;
	}
	int j = (i + 1) % path.size();

	// Get the points corresponding to i and j
	OrientedPoint p0 = path[i];
	OrientedPoint p1 = path[j];

	// Calculate the t value for distances
	float dist0 = distances[i];
	float dist1 = j == 0 ? totalLength : distances[j]; // Give total Length if after the lst point (since its a loop)
	float t = (dist - dist0) / (dist1 - dist0);

	return glm::mix(p0.orientation, p1.orientation, t);
}