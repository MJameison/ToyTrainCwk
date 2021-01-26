#include "Light.h"

#include <string>

Light::Light() {
	// default light settings emit no light
	diffuse = glm::vec3(0.0f);
	specular = glm::vec3(0.0f);
}

DirectionalLight::DirectionalLight() {
	direction = glm::vec3(0, -1, 0);
	ambient = glm::vec3(0.0f);
}

void DirectionalLight::addToShader(Shader *shader) {
	shader->setVec3("dirLight.direction", this->direction);
	shader->setVec3("dirLight.ambient", this->ambient);
	shader->setVec3("dirLight.diffuse", this->diffuse);
	shader->setVec3("dirLight.specular", this->specular);
}

SpotLight::SpotLight() {
	position = glm::vec3(0.0f);
	direction = glm::vec3(0, 0, -1);
	cutOff = glm::cos(glm::radians(15.0f));
	outerCutOff = glm::cos(glm::radians(17.5f));
}

void SpotLight::addToShader(Shader *shader) {
	shader->setVec3("spotLights[0].position", this->position);
	shader->setVec3("spotLights[0].direction", this->direction);
	shader->setVec3("spotLights[0].diffuse", this->diffuse);
	shader->setVec3("spotLights[0].specular", this->specular);
	shader->setFloat("spotLights[0].cutOff", this->cutOff);
	shader->setFloat("spotLight[0].outerCutOff", this->outerCutOff);
}

void SpotLight::addToShader(Shader *shader, unsigned int pos) {
	std::string prefix = "spotlights[" + std::to_string(pos) + "]";
	shader->setVec3(prefix + ".position", this->position);
	shader->setVec3(prefix + ".direction", this->direction);
	shader->setVec3(prefix + ".diffuse", this->diffuse);
	shader->setVec3(prefix + ".specular", this->specular);
	shader->setFloat(prefix + ".cutOff", this->cutOff);
	shader->setFloat(prefix + ".outerCutOff", this->outerCutOff);
}

PointLight::PointLight() {
	position = glm::vec3(0.0f);
}

void PointLight::addToShader(Shader *shader) {
	shader->setVec3("pointLights[0].position", this->position);
	shader->setVec3("pointLights[0].diffuse", this->diffuse);
	shader->setVec3("pointLights[0].specular", this->specular);
}

void PointLight::addToShader(Shader *shader, unsigned int pos) {
	std::string prefix = "pointLights[" + std::to_string(pos) + "]";
	shader->setVec3(prefix + ".position", this->position);
	shader->setVec3(prefix + ".diffuse", this->diffuse);
	shader->setVec3(prefix + ".specular", this->specular);
}
