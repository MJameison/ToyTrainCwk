#pragma once

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "Shader.h"


class Light {
public:
	Light();
	virtual void addToShader(Shader *shader) {};

	glm::vec3 diffuse;
	glm::vec3 specular;
};

class DirectionalLight : public Light {
public:
	DirectionalLight();
	void addToShader(Shader *shader) override;
	// Values
	glm::vec3 direction;
	glm::vec3 ambient; // Directional light controls scene ambient light
};

class PointLight : public Light {
public:
	PointLight();
	void addToShader(Shader *shader) override;
	void addToShader(Shader *shader, unsigned int pos);

	glm::vec3 position;
};

class SpotLight : public Light {
public:
	SpotLight();
	void addToShader(Shader *shader) override;
	void addToShader(Shader *shader, unsigned int pos);
	glm::vec3 position;
	glm::vec3 direction;
	float cutOff;
	float outerCutOff;
};
