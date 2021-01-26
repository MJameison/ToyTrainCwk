#pragma once
#include "Transform.h"
#include "Model.h"

class Scene;
//#include "Scene.h"

#include <glm/glm.hpp>

class GameObject {
public:
	virtual void Init();
	virtual void Update(float deltaTime);
	virtual void Draw(Shader *shader);


	~GameObject();

	Transform transform;
	Model *model;
	Scene *scene;
};

