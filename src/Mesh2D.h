#pragma once

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <vector>

#include "Mesh.h"

struct Vertex2D{
	glm::vec2 Position;
	glm::vec2 Normal;
	float TexPos;
};

struct OrientedPoint {
	glm::vec3 position;
	glm::quat orientation;
};

class Mesh2D {
public:
	Mesh2D() {};
	Mesh2D(std::vector<Vertex2D> vertices, std::vector<unsigned int> lines);
	unsigned int AddVertex(Vertex2D v);
	void AddLine(unsigned int v1, unsigned int v2);
	Mesh *Extrude(std::vector<OrientedPoint> path, float distances[]);

private:
	std::vector<Vertex2D> vertices;
	std::vector<unsigned int> lines;
};

