#include "Scene.h"

#include <iostream>

Scene::~Scene() {
	for (GameObject *go : gameObjects)
		delete go;

	glDeleteBuffers(1, &ubo);
}

Scene::Scene() {
	// Setup our ubo
	glGenBuffers(1, &ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, ubo, 0, 2 * sizeof(glm::mat4));

	activeCamera = nullptr;
	skybox = nullptr;
}

void Scene::Update(float deltaTime) {
	// Update the camera
	activeCamera->Update(deltaTime);

	for (GameObject *go : gameObjects)
		go->Update(deltaTime);
}

void Scene::Draw(Shader *shader) {
	shader->use();
	shader->setVec3("viewPos", activeCamera->transform.GetWorldPosition());
	if (!activeCamera) {
		std::cout << "No camera!" << std::endl;
		return;
	}
	// Set camera matrices
	// Update UBO
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(activeCamera->GetProjectionMatrix()));
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(activeCamera->GetViewMatrix()));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	// Draw all the gameobjects
	for (GameObject *go : gameObjects)
		go->Draw(shader);

	// Skybox drawn last
	glDepthFunc(GL_LEQUAL);
	skybox->Draw();
	glDepthFunc(GL_LESS);
}

void Scene::SetActiveCamera(Camera *camera) {
	this->activeCamera = camera;
}

void Scene::SetSkybox(Skybox *skybox) {
	this->skybox = skybox;
}

