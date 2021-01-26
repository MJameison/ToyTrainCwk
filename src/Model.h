#pragma once

#include <vector>

#include "Mesh.h"
#include "Material.h"

class Model {
public:
	Model();
	~Model();

	void Draw(Shader *shader);
	void AddMesh(Mesh *mesh, Material *mat = nullptr);

	void Clear();

	Material *GetMaterial(unsigned int index);

private:
	std::vector<std::pair<Mesh *, Material *>> meshes;
};

