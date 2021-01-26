#include "Material.h"

Material::Material() {
	ambientColor = glm::vec3(1.0f);
	useDiffuseTexture = false;
	diffuseColor = glm::vec3(1.0f);
	useSpecularTexture = false;
	specularColor = glm::vec3(1.0f);
	specularExponent = 64.0f;
	specularStrength = 1.0f;
	opacity = 1.0f;

	diffuseTexture = specularTexture = nullptr;

	uMapping = vMapping = TextureMapping::CLAMP;
}

void Material::Apply(Shader *shader) {
	shader->use();
	if (useDiffuseTexture) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseTexture->id);
		setTextureMapMode();
	}
	if (useSpecularTexture) {
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularTexture->id);
		setTextureMapMode();
	}

	shader->setVec3("material.ambient", ambientColor);
	shader->setBool("material.useDiffMap", useDiffuseTexture);
	shader->setVec3("material.diffuse", diffuseColor);
	shader->setInt("material.diffuseMap", 0);
	shader->setBool("material.useSpecMap", useSpecularTexture);
	shader->setVec3("material.specular", specularColor);
	shader->setInt("material.specularMap", 1);
	shader->setFloat("material.shininess", specularExponent);
	shader->setFloat("material.specularStrength", specularStrength);
	shader->setFloat("material.opacity", opacity);

	glActiveTexture(GL_TEXTURE0); // reset just in case
}

void Material::SetAmbientColor(glm::vec3 color) {
	this->ambientColor = color;
}

void Material::SetDiffuseColor(glm::vec3 color) {
	this->diffuseColor = color;
}

void Material::SetDiffuseTexture(Texture *tex) {
	this->useDiffuseTexture = tex;
	this->diffuseTexture = tex;
}

void Material::SetSpecularColor(glm::vec3 color) {
	this->specularColor = color;
}

void Material::SetSpecularTexture(Texture *tex) {
	this->useSpecularTexture = tex;
	this->specularTexture = tex;
}

void Material::SetShininess(float shininess) {
	this->specularExponent = shininess;
}

void Material::SetSpecularStrength(float strength) {
	this->specularStrength = strength;
}

void Material::SetOpacity(float opacity) {
	this->opacity = opacity;
}

void Material::SetMapping(TextureMapping uMap, TextureMapping vMap) {
	this->uMapping = uMap;
	this->vMapping = vMap;
}

void Material::setTextureMapMode() {
	GLint mapModeU = GL_CLAMP_TO_EDGE;
	switch (uMapping) {
	case TextureMapping::CLAMP:
		mapModeU = GL_CLAMP_TO_EDGE; break;
	case TextureMapping::MIRROR:
		mapModeU = GL_MIRRORED_REPEAT; break;
	case TextureMapping::REPEAT:
		mapModeU = GL_REPEAT; break;
	}
	GLint mapModeV = GL_CLAMP_TO_EDGE;
	switch (vMapping) {
	case TextureMapping::CLAMP:
		mapModeV = GL_CLAMP_TO_EDGE; break;
	case TextureMapping::MIRROR:
		mapModeV = GL_MIRRORED_REPEAT; break;
	case TextureMapping::REPEAT:
		mapModeV = GL_REPEAT; break;
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, mapModeU);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, mapModeV);
}
