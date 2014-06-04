#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Shader.h"
#include <glm/glm.hpp>
#include <iostream>
#include <fstream>
#include <string.h>
#include <FI/FreeImage.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <map>
#include "Model.h"
#include <CEGUI/CEGUI.h>
#include <CEGUI/RendererModules/OpenGL/GL3Renderer.h>
#include <CEGUI/RendererModules/OpenGL/GL.h>
#include "Camera.h"
#include "GUI.h"
#include "Skybox.h"
#include "Terrain.h"
#include <btBulletDynamicsCommon.h>
#include <BulletCollision/Gimpact/btGImpactCollisionAlgorithm.h>
#include "GameObject.h"
#include "Scene.h"


GLFWwindow* window;
GLuint vao2;
Camera camera;
float elapsedTime = 0;
float previousTime;

float mouseSpeed = 1.f;

#define X_AXIS glm::vec3(1.f,0.f,0.f)
#define Y_AXIS glm::vec3(0.f,1.f,0.f)
#define Z_AXIS glm::vec3(0.f,0.f,1.f)

bool wIsDown = false;
bool aIsDown = false;
bool sIsDown = false;
bool dIsDown = false;
bool qIsDown = false;
bool eIsDown = false;

double mouseXPos;
double mouseYPos;

float horizontalAngle = 3.14f;
float verticalAngle = 0.0f;

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS){
		switch (key){
		case GLFW_KEY_W:
			wIsDown = true;
			break;
		case GLFW_KEY_A:
			aIsDown = true;
			break;
		case GLFW_KEY_S:
			sIsDown = true;
			break;
		case GLFW_KEY_D:
			dIsDown = true;
			break;
		case GLFW_KEY_Q:
			qIsDown = true;
			break;
		case GLFW_KEY_E:
			eIsDown = true;
			break;
		case GLFW_KEY_F1:
			Scene::Instance->getMap()->SwapDebug();
			break;
		case GLFW_KEY_F2:
			Scene::Instance->SwapDebug();
			break;
		}
	}
	if (action == GLFW_RELEASE){
		switch (key){
		case GLFW_KEY_W:
			wIsDown = !true;
			break;
		case GLFW_KEY_A:
			aIsDown = !true;
			break;
		case GLFW_KEY_S:
			sIsDown = !true;
			break;
		case GLFW_KEY_D:
			dIsDown = !true;
			break;
		case GLFW_KEY_Q:
			qIsDown = !true;
			break;
		case GLFW_KEY_E:
			eIsDown = !true;
			break;
		}
	}
}

#pragma comment(lib, "CEGUIBase-0_d.lib")
int init(void){
	if (!glfwInit()){
		fprintf(stderr, "Failed to initialize GLFW\n");
		return -1;
	}
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);// Debug
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);// Normal
	window = glfwCreateWindow(1024, 768, "Exploration IIID", NULL, NULL);
	if (window == NULL){
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glewExperimental = true;
	if (glewInit() != GLEW_OK){
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetKeyCallback(window, key_callback);
	return 0;
}

void CheckInput(){
	glfwGetCursorPos(window, &mouseXPos, &mouseYPos);
	glfwSetCursorPos(window, 1024 / 2, 768 / 2);
	if (wIsDown)
		camera.Translate(10.f * elapsedTime * -1.f * Z_AXIS);
	if (aIsDown)
		camera.Translate(10.f * elapsedTime * -1.f * X_AXIS);
	if (sIsDown)
		camera.Translate(10.f * elapsedTime * 1.f * Z_AXIS);
	if (dIsDown)
		camera.Translate(10.f * elapsedTime * 1.f * X_AXIS);
	if (qIsDown)
		camera.Translate(10.f * elapsedTime * -1.f * Y_AXIS);
	if (eIsDown)
		camera.Translate(10.f * elapsedTime * 1.f * Y_AXIS);
}

int main(void){
	init();
	glfwSwapInterval(0);

	GUI gui;

	Shader modelShader = Shader("shaders/model.vert", "shaders/model.frag");
	Shader terrainShader = Shader("shaders/terrain.vert", "shaders/terrain.frag");
	Shader tileShader = Shader("shaders/tile.vert", "shaders/tile.frag");
	Shader skyboxShader = Shader("shaders/skybox.vert", "shaders/skybox.frag");
	Model::shader = &modelShader;
	Skybox::shader = &skyboxShader;
	Terrain::shader = &terrainShader;
	camera.shaders.push_back(&modelShader);
	camera.shaders.push_back(&terrainShader);
	camera.shaders.push_back(&tileShader);
	camera.shaders.push_back(&skyboxShader);

	Scene scene("data\\maps\\heightmap.bmp", "data/skybox/2/", glm::vec3(200.0f, 17.5f, 200.0f));

	GameObject tree("data\\models\\Trees\\6\\birch_01_a.obj", glm::vec2(45, 45), true);
	GameObject tree2("data\\models\\Trees\\6\\birch_01_a.obj", glm::vec2(55, 45), true);
	GameObject user("data\\models\\User\\User.x", glm::vec2(50, 50));
	//user.SetScale(glm::vec3(0.1, 0.1, 0.1));

	GameObject wolf("data\\models\\Wolf\\Wolf.obj", glm::vec2(50,50));

	previousTime = glfwGetTime();

	glClearColor(1.f, 1.f,1.f, 1.f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_CULL_FACE);
	do{
		CheckInput();
		elapsedTime = glfwGetTime() - previousTime;
		previousTime = glfwGetTime();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		Model::elapsedTime += elapsedTime;

		camera.Update();

		scene.Update();
		scene.Render();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindSampler(0, 0);
		gui.Render();

		glfwSwapBuffers(window);
		glfwPollEvents();
	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

	glfwTerminate();

	return 0;
}
/*
delete dynamicsWorld;
delete solver;
delete dispatcher;
delete collisionConfiguration;
delete broadphase;*/