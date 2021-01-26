#include "GameObject.h"

#include "Scene.h"

void GameObject::Init() {}
void GameObject::Update(float deltaTime) {}
void GameObject::Draw(Shader *shader) {
    if (!model)
        return;

    shader->use();
    glm::mat4 modelMat = transform.GetModelMatrix();
    shader->setMat4("model", modelMat);
    shader->setMat3("normalMatrix", glm::inverseTranspose(glm::mat3(modelMat)));
    model->Draw(shader);
}

GameObject::~GameObject() {
    delete model;
}

