#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <GL/glew.h>

struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
	glm::vec3 Tangent;
	glm::vec3 Bitangent;
};

/// <summary>
/// A mesh represents the vertexes and triangles of a complex 3D object.
/// </summary>
class Mesh {
public:
	Mesh();
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices);
	~Mesh();

	/// <summary>
	/// Load the mesh data and draws it to the screen.
	/// Assumes all shader uniforms are correctly set and that the correct shader is being used.
	/// </summary>
	void Draw();

	/// <summary>
	/// Reloads the mesh data after changes.
	/// </summary>
	void UpdateMesh();

	/// <summary>
	/// Adds a vertex to the mesh. This won't be used until it is added to a triangle.
	/// </summary>
	/// <param name="v">The vertex to be added</param>
	/// <returns>The index of the vertex</returns>
	unsigned int AddVertex(Vertex v);

	/// <summary>
	/// Adds a triangle to the mesh. This won't be drawn until UpdateMesh() is called.
	/// </summary>
	/// <param name="v1">The index of the first vertex</param>
	/// <param name="v2">The index of the second vertex</param>
	/// <param name="v3">The index of the third vertex</param>
	void AddTriangle(unsigned int v1, unsigned int v2, unsigned int v3);

private:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	GLuint VAO, VBO, EBO;
};

