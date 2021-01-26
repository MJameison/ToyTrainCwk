#pragma once

#include <vector>
#include <type_traits>

#include "Shader.h"
#include "Camera.h"

#include "Skybox.h"

#include <GL/glew.h>


class GameObject; // Forward declare to allow GameObjects to reference their scene

class Scene {
private:
	std::vector<GameObject *> gameObjects;
	Camera *activeCamera;
	Skybox *skybox;

	GLuint ubo;

public:
	~Scene();
	Scene();
	void Update(float deltaTime);
	void Draw(Shader *shader);
	void SetActiveCamera(Camera *camera);
	void SetSkybox(Skybox *skybox);

	template <typename T=GameObject>
	GameObject *CreateGameObject();
};

#include "GameObject.h"

template<typename T>
inline GameObject *Scene::CreateGameObject() {
	static_assert(std::is_base_of<GameObject, T>::value, "Type must inherit from GameObject");
	GameObject *obj = new T();
	obj->scene = this;
	obj->Init();
	gameObjects.push_back(obj);
	return obj;
}