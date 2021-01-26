#pragma once

#include "Shader.h"
#include <glm/glm.hpp>

#include "Texture.h"

enum class TextureMapping {
	CLAMP,
	MIRROR,
	REPEAT
};

class Material {
public:
	Material();

	/// <summary>
	/// Sets the values of the material struct in the given shader
	/// </summary>
	void Apply(Shader *shader);


	void SetAmbientColor(glm::vec3 color);
	void SetDiffuseColor(glm::vec3 color);
	void SetDiffuseTexture(Texture *tex);
	void SetSpecularColor(glm::vec3 color);
	void SetSpecularTexture(Texture *tex);
	void SetShininess(float shininess);
	void SetSpecularStrength(float strength);
	void SetOpacity(float opacity);
	void SetMapping(TextureMapping uMap, TextureMapping vMap);

private:
	void setTextureMapMode();

	// Material settings
	glm::vec3 ambientColor;

	bool useDiffuseTexture;
	glm::vec3 diffuseColor;
	Texture *diffuseTexture;

	bool useSpecularTexture;
	glm::vec3 specularColor;
	Texture *specularTexture;
	float specularExponent;
	float specularStrength;

	float opacity;

	TextureMapping uMapping;
	TextureMapping vMapping;
};

