#include "Train.h"
#include "StaticCamera.h"

#include "ResourceManager.h"
#include "FPSCamera.h"

#include "Scene.h"

#include <sstream>

#include <glm/glm.hpp>

void Train::Init() {
	// We hold the model in a child GameObject to allow it to be offset and scaled independently of track operations
	modelObj = scene->CreateGameObject();
	modelObj->model = ResourceManager::LoadModel("resources/train.obj");
	modelObj->transform.scale = glm::vec3(0.1f);
	modelObj->transform.SetEulerAngles(glm::vec3(0, glm::radians(180.0f), 0));
	modelObj->transform.position = glm::vec3(0.0f, 0.075f, 0.0f);
	modelObj->transform.parent = &this->transform;

	trainCamera = new StaticCamera();
	trainCamera->transform.parent = &this->transform;
	trainCamera->transform.position = glm::vec3(-0.1, 1, 0.9);
	scene->SetActiveCamera(trainCamera);

	freeCamera = new FPSCamera(0.01f);
	freeCamera->transform.position = glm::vec3(0.0f, 2.0f, 0.0f);

	track = (Track *) scene->CreateGameObject<Track>();
	// Some initial track
	controlPoints = {
		glm::vec3(-5, 0,  5),
		glm::vec3(5, 0,  5),
		glm::vec3(5, 0, -5),
		glm::vec3(-5, 0, -5)
	};

	track->CreateTrack(controlPoints);

	// Track movement initial values
	distance = 0.0f;
	speed = 4.0f;
	stopping = pressedSpace = false;
}

void Train::Update(float deltaTime) {
	// Camera Selection
	if (Input::IsKeyPressed('1'))
		scene->SetActiveCamera(trainCamera);
	if (Input::IsKeyPressed('2'))
		scene->SetActiveCamera(freeCamera);
	if (!Input::IsKeyPressed(' '))
		pressedSpace = false;
	if (!pressedSpace && Input::IsKeyPressed(' ')) {
		pressedSpace = true;
		stopping = !stopping;
	}

	// We allow control points to be defined in a text file we reload on key press
	if (Input::IsKeyPressed('r')) {
		loadControlPoints();
		track->CreateTrack(controlPoints);
	}

	// Speed control
	if (!stopping) {
		speed += deltaTime * (Input::IsKeyPressed('.') - Input::IsKeyPressed(','));
	}
	else {
		if (speed != 0) {
			float dir = speed > 0 ? 1.0f : -1.0f;
			speed = glm::clamp(speed * dir - deltaTime, 0.0f, 4.0f);
			speed *= dir;
		}
		else {
			// We've stopped so allow speed up again
			stopping = false;
		}
			
	}
	speed = glm::clamp(speed, -4.0f, 4.0f);

	// Update position on track
	distance += speed * deltaTime;
	transform.position = track->GetPosition(distance);
	transform.orientation = track->GetOrientation(distance);
}

void Train::loadControlPoints() {
	std::vector<std::string> fileContents = ResourceManager::ReadLines("resources/control_points.txt");
	if (fileContents.size() < 3) {
		std::cerr << "File doesn't define at least 4 control points" << std::endl;
	}
	controlPoints.clear();
	size_t line_no = 0;
	for (std::string &line : fileContents) {
		line_no++;
		float val;
		std::stringstream ss(line);

		float points[] = { 0.0f, 0.0f, 0.0f };
		size_t i = 0;
		while (ss >> val) {
			points[i++] = val;
			if (ss.peek() == ',') ss.ignore();
			if (i > 2) break;
		}
		if (i != 3) {
			std::cerr << "Error: line " << line_no << " has " << i << " values. Needs 3." << std::endl;
		}
		glm::vec3 point = glm::vec3(points[0], points[1], points[2]);
		controlPoints.push_back(point);
	}
}
