#ifndef GUI_H
#define GUI_H

#include "GL/glew.h"
#include "GLFW\glfw3.h"
#include <CEGUI/CEGUI.h>
#include <CEGUI/RendererModules/OpenGL/GL3Renderer.h>
#include <CEGUI/RendererModules/OpenGL/GL.h>


class GUI{
public:
	GUI();
	void Render();
private:
	double frameTime;
	double FPS;
	double lastTime;
	int nbFrames = 0;
	CEGUI::Window* myRoot;
	void CalculateDebug();
};

#endif