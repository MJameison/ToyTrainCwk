#include "FPSCamera.h"
#include "GL/glut.h"

#include <iostream>

FPSCamera::FPSCamera(float sensitivity) {
    this->sensitivity = sensitivity;
    pitch = yaw = 0.0f;
    transform = Transform(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f), glm::vec3(1.0f));
    cameraSpeed = 4.0f;
}

void FPSCamera::Update(float deltaTime) {
    // Rotation code
    if (Input::IsMousePressed(1)) {
        glm::vec2 offset = Input::GetMouseOffset();
        offset *= sensitivity;
        pitch -= offset.y;
        yaw -= offset.x;
        // Clamp pitch and keep yaw in -180 to 180
        pitch = glm::clamp(pitch, glm::radians(-90.0f), glm::radians(90.0f));

        // Update transform
        transform.orientation = glm::quat(glm::vec3(pitch, yaw, 0.0f));
    }

    // Movement code
    glm::vec3 move = glm::vec3(0.0f);
    move.x = Input::IsKeyPressed('d') - Input::IsKeyPressed('a');
    move.y = Input::IsKeyPressed('e') - Input::IsKeyPressed('q');
    move.z = Input::IsKeyPressed('s') - Input::IsKeyPressed('w');
    // Rotate to yaw direction (no vertical movement)
    move = glm::quat(glm::vec3(0.0f, yaw, 0.0f)) * move;
    // Normalize to ensure fixed speed
    if (glm::length(move) > 0.1f) {
        move = glm::normalize(move);
        move *= deltaTime * cameraSpeed;
        transform.position += move;
    }
}

glm::mat4 FPSCamera::GetViewMatrix() {
    glm::mat4 rotationMatrix = glm::mat4_cast(glm::inverse(transform.orientation));
    glm::mat4 translateMatrix = glm::translate(-transform.position);
    return rotationMatrix * translateMatrix;
}

glm::mat4 FPSCamera::GetProjectionMatrix() {
    float viewWidth = (float) glutGet(GLUT_WINDOW_WIDTH);
    float viewHeight = (float) glutGet(GLUT_WINDOW_HEIGHT);
    return glm::perspectiveFov(45.0f, viewWidth, viewHeight, 0.1f, 100.0f);
}

