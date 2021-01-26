#include "Shader.h"

#include "ResourceManager.h"

/// <summary>
/// Creates a shader program. Compiles the code found at the source paths and sets the ID field to the resulting program's ID
/// </summary>
/// <param name="vertex_source_path">The file path to the vertex shader source</param>
/// <param name="fragment_source_path">The file path to the fragment shader source</param>
Shader::Shader(const char* vertex_source_path, const char* fragment_source_path) {
	std::string vertex_source = ResourceManager::ReadFile(vertex_source_path);
	std::string fragment_source = ResourceManager::ReadFile(fragment_source_path);
	
	GLuint vShader, fShader;
	GLint success;
	GLchar log[1024];

	const char *v_code = vertex_source.c_str();
	const char *f_code = fragment_source.c_str();

	vShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vShader, 1, &v_code, nullptr);
	glCompileShader(vShader);

	glGetShaderiv(vShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vShader, 1024, nullptr, log);
		std::cerr << "Error: Vertex shader:\n" << log << std::endl;
	}

	fShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fShader, 1, &f_code, nullptr);
	glCompileShader(fShader);

	glGetShaderiv(fShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fShader, 1024, nullptr, log);
		std::cerr << "Error: Fragment shader:\n" << log << std::endl;
	}

	ID = glCreateProgram();
	glAttachShader(ID, vShader);
	glAttachShader(ID, fShader);
	glLinkProgram(ID);

	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(ID, 1024, nullptr, log);
		std::cerr << "Error: Shader linking:\n" << log << std::endl;
	}

	glDeleteShader(vShader);
	glDeleteShader(fShader);
}

void Shader::use() {
	glUseProgram(ID);
}

void Shader::setBool(const std::string& name, GLboolean value) const {
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (GLint)value);
}

void Shader::setInt(const std::string& name, GLint value) const {
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, GLfloat value) const {
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setVec2(const std::string& name, const glm::vec2& value) const {
	glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void Shader::setVec2(const std::string& name, float x, float y) const {
	glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) const {
	glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void Shader::setVec3(const std::string& name, float x, float y, float z) const {
	glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}

void Shader::setVec4(const std::string& name, const glm::vec4& value) const {
	glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void Shader::setVec4(const std::string& name, float x, float y, float z, float w) const {
	glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
}

void Shader::setMat2(const std::string& name, const glm::mat2& mat) const {
	glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setMat3(const std::string& name, const glm::mat3& mat) const {
	glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setMat4(const std::string& name, const glm::mat4& mat) const {
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
