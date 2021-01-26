#include "Model.h"

Model::Model() {
}

Model::~Model() {
	for (std::pair<Mesh *, Material *> mm : meshes) {
		delete mm.first;
		delete mm.second;
	}
}

void Model::Draw(Shader *shader) {
	for (std::pair<Mesh *, Material *> mm : meshes) {
		shader->use();
		mm.second->Apply(shader);
		mm.first->Draw();
	}
}

void Model::AddMesh(Mesh *mesh, Material *mat) {
	if (!mat)
		mat = new Material(); // if not given a material use the default
	meshes.push_back({ mesh, mat });
}

void Model::Clear() {
	for (std::pair<Mesh *, Material *> mm : meshes) {
		delete mm.first;
		delete mm.second;
	}
	meshes.clear();
}

Material *Model::GetMaterial(unsigned int index) {
	return meshes[index].second;
}
