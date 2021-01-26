#pragma once

#include <glm/glm.hpp>

void keyboardPressCallback(unsigned char key, int, int);
void keyboardReleaseCallback(unsigned char key, int, int);
void mouseButtonPressCallback(int, int, int, int);
void mouseMotionCallback(int, int);

class Input {
private:
	Input(); // No instantiation

public:
	static void Update();
	static bool IsKeyPressed(unsigned char key);
	static bool IsMousePressed(int button);
	static glm::vec2 GetMousePosition();
	static glm::vec2 GetMouseOffset();
};

