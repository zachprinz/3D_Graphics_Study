#ifndef SKYBOX_H
#define SKYBOX_H

#include <iostream>
#include <string>
#include <GL/glew.h>
#include "VBO.h"
#include "Texture.h"
#include "Shader.h"

using namespace std;

class Skybox{
public:
	Skybox();
	void Load(string directory, string front, string back, string left, string right, string top, string bottom);
	static Shader* shader;
	void Render();
	void Update();
private:
	static GLuint modelID;
	GLuint vaoID;
	VBO vbo;
	Texture textures[6];
	string front, back, left, right, top, bottom;
};

#endif