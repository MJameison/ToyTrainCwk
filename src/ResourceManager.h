#pragma once

#include <GL/glew.h>

#include <glm/glm.hpp>

#include <string>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Texture.h"
#include "Model.h"

using namespace std;

class ResourceManager {

public:
	static void Init();
	static Texture *LoadTexture(const string &path);
	static string ReadFile(const string &filePath);
	static vector<string> ReadLines(const string &filePath);
	static Model *LoadModel(const string &path);

private:
	static void processNode(aiNode *node, const aiScene *scene, Model *model, const string dir);
	static std::pair<Mesh *, Material *> processMesh(aiMesh *mesh, const aiScene *scene, const string dir);
	static Material *loadMaterial(aiMaterial *mat, string directory);
};

