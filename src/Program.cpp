#include <GL/glew.h>
#include <GL/glut.h>

#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "ResourceManager.h"

#include "stb_image.h"

#include "Shader.h"
#include "Camera.h"
#include "FPSCamera.h"
#include "Input.h"
#include "Light.h"
#include "Skybox.h"
#include "Mesh.h"
#include "Material.h"
#include "Model.h"
#include "GameObject.h"
#include "Track.h"
#include "Scene.h"
#include "Train.h"

// Scene details
float screenWidth;
float screenHeight;

// Post-process elements
Shader *postProcessShader;
GLuint framebuffer;
GLuint multisampleTextureBuffer;
GLuint rbo;
GLuint postProcessQuadVAO;
GLuint postProcessQuadVBO;

GLuint intermediateFBO;
GLuint screenTexture;

//float gamma = 2.0f;

Scene *scene;

// Lights
DirectionalLight dirLight;
SpotLight spotLight;
std::vector<PointLight> pointLights;

// Shaders
Shader *blinnPhongShader;

glm::vec3 pointLightPositions[] = {
	glm::vec3(-15.0f, 1.0f, 0.0f),
	glm::vec3(-5.0f, 1.0f, 0.0f),
	glm::vec3(5.0f, 1.0f, 0.0f),
	glm::vec3(15.0f, 1.0f, 0.0f)
};

void changeViewPort(int w, int h) {
	glViewport(0, 0, w, h);
	screenWidth = (float) w;
	screenHeight = (float) h;

	// Delete existing GL resources
	glDeleteFramebuffers(1, &framebuffer);
	glDeleteFramebuffers(1, &intermediateFBO);
	glDeleteTextures(1, &multisampleTextureBuffer);
	glDeleteRenderbuffers(1, &rbo);
	glDeleteTextures(1, &screenTexture);

	// MSAA framebuffer
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	// create a multisampled color attachment texture
	glGenTextures(1, &multisampleTextureBuffer);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, multisampleTextureBuffer);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB, w, h, GL_TRUE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, multisampleTextureBuffer, 0);
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, w, h);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		cout << "Error: Framebuffer is not complete!" << endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Postprocess framebuffer
	glGenFramebuffers(1, &intermediateFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, intermediateFBO);
	glGenTextures(1, &screenTexture);
	glBindTexture(GL_TEXTURE_2D, screenTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenTexture, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		cout << "Error: Intermediate framebuffer is not complete!" << endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void idleCallback() {
	// Always redraw the frame -- no delays
	glutPostRedisplay();
}

void Init() {
	ResourceManager::Init();

	// Load shaders
	blinnPhongShader = new Shader("shaders/material.vert", "shaders/material.frag");

	// Load the primitive quad
	Mesh *quadMesh = new Mesh(
		{
			{glm::vec3(-100, 0.0, -100), glm::vec3(0, 1, 0), glm::vec2(0, 0), glm::vec3(), glm::vec3()},
			{glm::vec3( 100, 0.0, -100), glm::vec3(0, 1, 0), glm::vec2(100, 0), glm::vec3(), glm::vec3()},
			{glm::vec3(-100, 0.0,  100), glm::vec3(0, 1, 0), glm::vec2(0, 100), glm::vec3(), glm::vec3()},
			{glm::vec3( 100, 0.0,  100), glm::vec3(0, 1, 0), glm::vec2(100, 100), glm::vec3(), glm::vec3()}
		},
		{2, 1, 0, 2, 3, 1}
	);

	scene = new Scene();
	scene->SetSkybox(new Skybox("resources/cubemap/", ".jpg"));
	scene->CreateGameObject<Train>();

	// Create a quad to act as the floor
	GameObject *floorQuad = scene->CreateGameObject();
	Material *floorMat = new Material();
	floorMat->SetMapping(TextureMapping::REPEAT, TextureMapping::REPEAT);
	floorMat->SetDiffuseTexture(ResourceManager::LoadTexture("resources/ground_diffuse.jpg"));
	floorMat->SetSpecularTexture(ResourceManager::LoadTexture("resources/ground_specular.png"));
	floorMat->SetSpecularColor(glm::vec3(0.5f));
	floorQuad->model = new Model();
	floorQuad->model->AddMesh(quadMesh, floorMat);

	// Light setup
	// directional
	dirLight.direction = glm::normalize(glm::vec3(0.2f, -.3f, 0.3f));
	dirLight.ambient = glm::vec3(0.05f);
	dirLight.diffuse = glm::vec3(0.8f);
	dirLight.specular = glm::vec3(0.5f);
	// spot light
	//spotLight.diffuse = glm::vec3(1.0f);
	//spotLight.specular = glm::vec3(1.0f);
	spotLight.position = glm::vec3(0.0f, 2.0f, 0.0f);
	spotLight.direction = glm::normalize(glm::vec3(1.0f, -1.0f, 0.0f));
	// point lights
	for (unsigned int i = 0; i < 4; ++i) {
		PointLight pl;
		//pl.diffuse = glm::vec3(1.0f);
		//pl.specular = glm::vec3(0.25f * (i+1));
		pl.position = pointLightPositions[i];
		pointLights.push_back(pl);
	}

	// Post Process Setup
	float postProcessVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
		// positions   // texCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
	};

	postProcessShader = new Shader("shaders/postProcess.vert", "shaders/postProcess.frag");
	postProcessShader->setInt("screenTexture", 0);

	glGenVertexArrays(1, &postProcessQuadVAO);
	glGenBuffers(1, &postProcessQuadVBO);
	glBindVertexArray(postProcessQuadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, postProcessQuadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(postProcessVertices), &postProcessVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) (2 * sizeof(float)));
}

void Update(float deltaTime) {
	// Keyboard events
	if (Input::IsKeyPressed(27)) {
		exit(0);
	}
	Input::Update();
	scene->Update(deltaTime);

	//spotLight.position = glm::vec3(camera.transform.position);
	//spotLight.direction = glm::normalize(camera.transform.orientation) * glm::vec3(0, 0, -1);
	
	//gamma += (Input::IsKeyPressed(']') - Input::IsKeyPressed('[')) * deltaTime;
	//gamma = glm::clamp(gamma, 0.0f, 4.0f);
}

float lastFrame;
void render() {
	float currentFrame = (float)glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
	float deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	Update(deltaTime);

	blinnPhongShader->use();

	// Lights
	dirLight.addToShader(blinnPhongShader);
	spotLight.addToShader(blinnPhongShader);
	for (unsigned int i = 0; i < 4; ++i) {
		pointLights[i].addToShader(blinnPhongShader, i);
	}
	
	glViewport(0, 0, screenWidth, screenHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glEnable(GL_DEPTH_TEST);
	// Clear the buffer
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	// Draw the scene
	scene->Draw(blinnPhongShader);

	// PostProcessing here

	// Blit multisampled buffers to color buffer of intermediate FBO
	glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, intermediateFBO);
	glBlitFramebuffer(0, 0, screenWidth, screenHeight, 0, 0, screenWidth, screenHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	// Now render quad with the visuals
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);

	postProcessShader->use();
	postProcessShader->setFloat("gamma", 2.2f);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(postProcessQuadVAO);
	glBindTexture(GL_TEXTURE_2D, screenTexture);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glutSwapBuffers();
}

int main(int argc, char* argv[]) {

	// GLUT setup
	glutInit(&argc, argv);
	glutInitWindowSize(800, 600);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE);
	//glutInitDisplayMode(0);
	glutCreateWindow("Initializing...");


	// GLEW setup
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		std::cerr << "Error: " << glewGetErrorString(err);
		return 1;
	}
	std::cout << "Using GLEW " << glewGetString(GLEW_VERSION) << std::endl;

	// GLUT callbacks
	// Graphics callbacks
	glutReshapeFunc(changeViewPort);
	glutDisplayFunc(render);
	glutIdleFunc(idleCallback);

	// Input callbacks (Defined in Input.h)
	glutKeyboardFunc(keyboardPressCallback);
	glutKeyboardUpFunc(keyboardReleaseCallback);
	glutMouseFunc(mouseButtonPressCallback);
	glutMotionFunc(mouseMotionCallback);
	glutPassiveMotionFunc(mouseMotionCallback);

	// OpenGL enables
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_STENCIL_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);

	//glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	//glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Init();

	glutSetWindowTitle("Toy Train | u1808172");
	
	glutMainLoop();

	return 0;
}