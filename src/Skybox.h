#pragma once

#include "Shader.h"

#include <string>

class Skybox {
public:
	Skybox(std::string directory, std::string extension);
	void Draw();
	~Skybox();
private:
	GLuint textureID;
	GLuint vao, vbo;
	Shader *skyboxShader;
};

