#include "ResourceManager.h"

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif

#include "stb_image.h"
#include <iostream>
#include <fstream>
#include <sstream>

// Store all loaded textures to save time
vector<Texture *> textures_loaded;

void ResourceManager::Init() {
	stbi_set_flip_vertically_on_load(true);
}

Texture *ResourceManager::LoadTexture(const string& path) {
	// Check if texture is already loaded
	for (unsigned int i = 0; i < textures_loaded.size(); ++i) {
		if (strcmp(textures_loaded[i]->path.data(), path.data()) == 0) {
			return textures_loaded[i];
		}
	}

	GLuint textureID;
	glGenTextures(1, &textureID);

	// load and generate the texture
	int width, height, channels;
	GLubyte* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
	if (data) {
		GLenum format = GL_RED;
		GLenum internalFormat = GL_RED;
		if (channels == 3) {
			format = GL_RGB;
			internalFormat = GL_SRGB;
		}
		else if (channels == 4) {
			format = GL_RGBA;
			internalFormat = GL_SRGB_ALPHA;
		}

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		cout << "Loaded \"" << path << "\" (" << width << ", " << height << ") channels: " << channels << endl;
	}
	else {
		cerr << "Error: Failed to load texture: \"" << path << "\"" << endl;
	}
	stbi_image_free(data);
	// Copy into texture struct
	Texture *tex = new Texture();
	tex->id = textureID;
	tex->path = path;

	textures_loaded.push_back(tex);
	return tex;
}

string ResourceManager::ReadFile(const string& filePath) {

	ifstream file;
	string res;
	file.exceptions(ifstream::failbit | ifstream::badbit);
	try {
		file.open(filePath);

		stringstream stream;
		stream << file.rdbuf();

		file.close();

		res = stream.str();
	}
	catch (ifstream::failure e) {
		cerr << "Error: File not read" << endl;
	}
	return res;
}

vector<string> ResourceManager::ReadLines(const string &filePath) {
	vector<string> lines;

	ifstream file(filePath);
	if (!file) {
		cerr << "Error: Cannot open file: " << filePath << endl;
		return lines;
	}

	string str;
	while (getline(file, str)) {
		if (str.size() > 0)
			lines.push_back(str);
	}
	file.close();
	return lines;
}

Model *ResourceManager::LoadModel(const string& path) {
	Model *model = new Model();

	cout << "Loading " << path << "..." << endl;
	Assimp::Importer importer;
	const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_GenUVCoords);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		cerr << "Error: Assimp: " << importer.GetErrorString() << endl;
		return nullptr;
	}
	// Get the directory
	string directory = path.substr(0, path.find_last_of('/')) + "/";

	processNode(scene->mRootNode, scene, model, directory);

	cout << "Finsished loading " << path << endl;

	return model;
}

void ResourceManager::processNode(aiNode *node, const aiScene *scene, Model *model, const string dir) {
	for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
		auto mm = processMesh(mesh, scene, dir);
		model->AddMesh(mm.first, mm.second);
	}
	for (unsigned int i = 0; i < node->mNumChildren; ++i) {
		processNode(node->mChildren[i], scene, model, dir);
	}
}


std::pair<Mesh *, Material *> ResourceManager::processMesh(aiMesh *mesh, const aiScene *scene, const string dir) {
	// data to fill
	vector<Vertex> vertices;
	vector<unsigned int> indices;

	// for each vertex
	for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
		Vertex vertex;
		glm::vec3 vector; // Placeholder vector to copy data
		// position
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.Position = vector;
		// normal
		if (mesh->HasNormals()) {
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.Normal = vector;
		}
		// texture coords
		if (mesh->mTextureCoords[0]) { // Can be up to 8 but we only use the first
			glm::vec2 vec;
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec;
			// tangent
			vector.x = mesh->mTangents[i].x;
			vector.y = mesh->mTangents[i].y;
			vector.z = mesh->mTangents[i].z;
			vertex.Tangent = vector;
			// bitangent
			vector.x = mesh->mBitangents[i].x;
			vector.y = mesh->mBitangents[i].y;
			vector.z = mesh->mBitangents[i].z;
			vertex.Bitangent = vector;
		}
		else
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);

		vertices.push_back(vertex);
	}
	// now for the triangles
	for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; ++j)
			indices.push_back(face.mIndices[j]);
	}
	// now the material
	aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
	Material *mat = loadMaterial(material, dir);

	return { new Mesh(vertices, indices), mat };
}

Material *ResourceManager::loadMaterial(aiMaterial *mat, string directory) {
	Material *m = new Material();

	// Ambient properties
	aiColor3D ambientColor (0.0f, 0.0f, 0.0f);
	mat->Get(AI_MATKEY_COLOR_AMBIENT, ambientColor);
	m->SetAmbientColor(glm::vec3(ambientColor.r, ambientColor.g, ambientColor.b));

	// Diffuse properties
	aiColor3D diffuseColor(0.0f, 0.0f, 0.0f);
	mat->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor);
	m->SetDiffuseColor(glm::vec3(diffuseColor.r, diffuseColor.g, diffuseColor.b));

	aiString diffuseTexName;
	mat->GetTexture(aiTextureType_DIFFUSE, 0, &diffuseTexName);
	if (diffuseTexName.length > 0) {
		m->SetDiffuseTexture(LoadTexture(directory + diffuseTexName.C_Str()));
	}
		
	// Specular properties
	aiColor3D specularColor(0.0f, 0.0f, 0.0f);
	mat->Get(AI_MATKEY_COLOR_SPECULAR, specularColor);
	m->SetSpecularColor(glm::vec3(specularColor.r, specularColor.g, specularColor.b));

	aiString specTexName;
	mat->GetTexture(aiTextureType_DIFFUSE, 0, &specTexName);
	if (diffuseTexName.length > 0) {
		m->SetSpecularTexture(LoadTexture(directory + specTexName.C_Str()));
	}

	float shininess;
	aiGetMaterialFloat(mat, AI_MATKEY_SHININESS, &shininess);
	m->SetShininess(shininess);

	float strength;
	aiGetMaterialFloat(mat, AI_MATKEY_SHININESS, &strength);
	m->SetSpecularStrength(strength);

	// Opacity
	float opacity;
	aiGetMaterialFloat(mat, AI_MATKEY_OPACITY, &opacity);
	m->SetOpacity(opacity);

	// Get render type
	int mode;
	aiGetMaterialInteger(mat, AI_MATKEY_SHADING_MODEL, &mode);
	if (!(mode == aiShadingMode_Phong || mode == aiShadingMode_Blinn))
		m->SetSpecularStrength(0.0f);

	// Set mapping mode
	aiTextureMapping mapModeU;
	mat->Get(AI_MATKEY_MAPPINGMODE_U(aiTextureType_DIFFUSE, 0), mapModeU);
	aiTextureMapping mapModeV;
	mat->Get(AI_MATKEY_MAPPINGMODE_V(aiTextureType_DIFFUSE, 0), mapModeV);

	TextureMapping uMap = TextureMapping::CLAMP;
	TextureMapping vMap = TextureMapping::CLAMP;

	if (mapModeU == aiTextureMapMode_Wrap)
		uMap = TextureMapping::REPEAT;
	else if (mapModeU == aiTextureMapMode_Mirror)
		uMap = TextureMapping::MIRROR;

	if (mapModeV == aiTextureMapMode_Wrap)
		vMap = TextureMapping::REPEAT;
	else if (mapModeV == aiTextureMapMode_Mirror)
		vMap = TextureMapping::MIRROR;

	m->SetMapping(uMap, vMap);

	return m;
}