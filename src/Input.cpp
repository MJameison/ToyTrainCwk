#include "Input.h"

#include <GL/glut.h>

bool keyPressed[256];
bool mousePressed[3];

glm::vec2 mousePos;
glm::vec2 lastOffsetPos;
glm::vec2 mouseOffset;

/* 
 * Helper functions
 * Valid functions for GLUT callbacks to seperate the glut input handling from
 * the rest of the codebase
 */

void keyboardPressCallback(unsigned char key, int, int) {
	keyPressed[key] = true;
}

void keyboardReleaseCallback(unsigned char key, int, int) {
	keyPressed[key] = false;
}

void mouseButtonPressCallback(int button, int state, int, int) {
	if (button == GLUT_LEFT_BUTTON)
		mousePressed[0] = state == GLUT_DOWN;
	else if (button == GLUT_RIGHT_BUTTON)
		mousePressed[1] = state == GLUT_DOWN;
	else if (button == GLUT_MIDDLE_BUTTON)
		mousePressed[2] = state == GLUT_DOWN;
}

void mouseMotionCallback(int x, int y) {
	mousePos = glm::vec2(x, y);
}

void Input::Update() {
	// Update mouse offset
	mouseOffset = mousePos - lastOffsetPos;
	lastOffsetPos = mousePos;
}

bool Input::IsKeyPressed(unsigned char key) {
	return keyPressed[key];
}

bool Input::IsMousePressed(int button) {
	if (button >= 0 && button < 3)
		return mousePressed[button];
	return false;
}

glm::vec2 Input::GetMousePosition() {
	return mousePos;
}

glm::vec2 Input::GetMouseOffset() {
	return mouseOffset;
}

