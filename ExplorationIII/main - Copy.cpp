#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Shader.hpp"
#include <glm/glm.hpp>
#include <iostream>
#include <fstream>
#include <string.h>
#include <stdarg.h>
#include <FI/FreeImage.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <map>

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

GLFWwindow* window;
GLuint vao2;

// Cube vertices
GLfloat cubeVertices[] = {
	-0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
	0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
	0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
	0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
	-0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,

	-0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
	0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
	0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
	0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
	-0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
	-0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,

	-0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
	-0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
	-0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
	-0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,

	0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
	0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
	0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
	0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
	0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
	0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
	0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
	0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
	0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
	-0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,

	-0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
	0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
	0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
	0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
	-0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
	-0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,

	-1.0f, -1.0f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
	1.0f, -1.0f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
	1.0f, 1.0f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
	1.0f, 1.0f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
	-1.0f, 1.0f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
	-1.0f, -1.0f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f
};

// Quad vertices
GLfloat quadVertices[] = {
	-1.0f, 1.0f, 0.0f, -1.0f,1.0f,0.0f,0.0f,0.0f,
	1.0f, 1.0f, 1.0f, 1.0f,1.0f,0.0f,0.0f,0.0f,
	1.0f, -1.0f, 1.0f, 1.0f,1.0f,0.0f,0.0f,0.0f,
	1.0f, -1.0f, 1.0f, 1.0f,1.0f,0.0f,0.0f,0.0f,
	-1.0f, -1.0f, 0.0f, -1.0f,-1.0f,0.0f,0.0f,0.0f,
	-1.0f, 1.0f, 0.0f, -1.0f,1.0f0.0f,0.0f,0.0f
};

int init(void){
	if (!glfwInit()){
		fprintf(stderr, "Failed to initialize GLFW\n");
		return -1;
	}
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(1024, 768, "Exploration IIID", NULL, NULL);
	if (window == NULL){
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);
	return 0;
}

void loadTexture(const GLchar* path, GLuint* tex){
	FIBITMAP* bitmap = FreeImage_Load(FreeImage_GetFileType(path, 0), path);
	BYTE* dataPointer = FreeImage_GetBits(bitmap);
	int nWidth = FreeImage_GetWidth(bitmap);
	int nHeight = FreeImage_GetHeight(bitmap);
	int iBPP = FreeImage_GetBPP(bitmap);
	glGenTextures(1, tex);
	glBindTexture(GL_TEXTURE_2D, *tex);
	int iFormat = iBPP == 24 ? GL_BGR : iBPP == 8 ? GL_LUMINANCE : 0;
	int iInternalFormat = iBPP == 24 ? GL_RGB : GL_DEPTH_COMPONENT;
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, nWidth, nHeight, 0, iFormat, GL_UNSIGNED_BYTE, dataPointer);
	glGenerateMipmap(GL_TEXTURE_2D);
	FreeImage_Unload(bitmap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

int main(void)
{
	//----------------------------------------------------------------//
	//------------------Init GLFW and GLEW (OpenGL)-------------------//
	//----------------------------------------------------------------//

	init();

	//----------------------------------------------------------------//
	//-----------Create a vertex Array to hold preferences------------//
	//----------------------------------------------------------------//

	GLuint vaoCube, vaoQuad;
	glGenVertexArrays(1, &vaoCube);
	glGenVertexArrays(1, &vaoQuad);
	//Not Binding Yet

	//----------------------------------------------------------------//
	//--------------Create a Vertex Buffer for the Cube---------------//
	//----------------------------------------------------------------//

	GLuint vboCube, vboQuad;
	glGenBuffers(1, &vboCube);
	glGenBuffers(1, &vboQuad);
	glBindBuffer(GL_ARRAY_BUFFER, vboCube);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, vboQuad);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

	//----------------------------------------------------------------//
	//--------------------Create the Shader Program-------------------//
	//----------------------------------------------------------------//

	GLuint sceneProgram = LoadShaders("vertex-shader.txt", "fragment-shader-two.txt");
	GLuint screenProgram = LoadShaders("post-vertex.txt", "post-fragment.txt");
	//Not binding programs yet.

	//----------------------------------------------------------------//
	//--------------Set Up the Programs Vertex Attribs----------------//
	//----------------------------------------------------------------//'

	//Scene Program
	glBindVertexArray(vaoCube);
	glBindBuffer(GL_ARRAY_BUFFER, vboCube);
	GLint posAttrib = glGetAttribLocation(sceneProgram, "position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
	GLint colorAttrib = glGetAttribLocation(sceneProgram, "color");
	glEnableVertexAttribArray(colorAttrib);
	glVertexAttribPointer(colorAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	GLint texAttrib = glGetAttribLocation(sceneProgram, "texcoord");
	glEnableVertexAttribArray(texAttrib);
	glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));

	//Screen Program
	glBindVertexArray(vaoQuad);
	glBindBuffer(GL_ARRAY_BUFFER, vboQuad);
	GLint scenePosAttrib = glGetAttribLocation(screenProgram, "position");
	glEnableVertexAttribArray(scenePosAttrib);
	glVertexAttribPointer(scenePosAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	GLint sceneTexAttrib = glGetAttribLocation(screenProgram, "texcoord");
	glEnableVertexAttribArray(sceneTexAttrib);
	glVertexAttribPointer(sceneTexAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));

	//----------------------------------------------------------------//
	//--------------------------Load Texture--------------------------//
	//----------------------------------------------------------------//

	GLuint tex;
	loadTexture("crate.jpg", &tex);

	//----------------------------------------------------------------//
	//--------------------------Set Uniforms--------------------------//
	//----------------------------------------------------------------//

	glUseProgram(sceneProgram);
	glUniform1i(glGetUniformLocation(sceneProgram, "tex"), 0);

	glUseProgram(screenProgram);
	glUniform1i(glGetUniformLocation(screenProgram, "texFramebuffer"), 0);

	//----------------------------------------------------------------//
	//----------------------Create Frame Buffer-----------------------//
	//----------------------------------------------------------------//

	// Create frame buffer
	GLuint frameBuffer;
	glGenFramebuffers(1, &frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

	// Create texture to hold color buffer
	GLuint texColorBuffer;
	glGenTextures(1, &texColorBuffer);
	glBindTexture(GL_TEXTURE_2D, texColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);

	// Create Renderbuffer Object to hold depth and stencil buffers
	GLuint rboDepthStencil;
	glGenRenderbuffers(1, &rboDepthStencil);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepthStencil);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 800, 600);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboDepthStencil);

	//----------------------------------------------------------------//
	//-----------------Create Model, View, Projection-----------------//
	//----------------------------------------------------------------//

	// Set up projection
	GLint uniModel = glGetUniformLocation(sceneProgram, "trans");

	glm::mat4 view = glm::lookAt(glm::vec3(2.5f, 2.5f, 2.0f),glm::vec3(0.0f, 0.0f, 0.0f),glm::vec3(0.0f, 0.0f, 1.0f));
	GLint uniView = glGetUniformLocation(sceneProgram, "view");
	glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));

	glm::mat4 proj = glm::perspective(45.0f, 800.0f / 600.0f, 1.0f, 10.0f);
	GLint uniProj = glGetUniformLocation(sceneProgram, "proj");
	glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));

	GLint uniColor = glGetUniformLocation(sceneProgram, "overrideColor");

	//----------------------------------------------------------------//
	//---------------------Try Loading A 3D Model---------------------//
	//----------------------------------------------------------------//

	float time = 0.0f;
	do{
		//Increment time
		time += 0.012f;

		// Bind our framebuffer and draw 3D scene (spinning cube)
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
		glBindVertexArray(vaoCube);
		glEnable(GL_DEPTH_TEST);
		glUseProgram(sceneProgram);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex);

		// Clear the screen to white
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Calculate transformation
		glm::mat4 model;
		model = glm::rotate(model,time * 180.0f,glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));

		// Draw cube
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glEnable(GL_STENCIL_TEST);

		// Draw floor
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glStencilMask(0xFF);
		glDepthMask(GL_FALSE);
		glClear(GL_STENCIL_BUFFER_BIT);

		glDrawArrays(GL_TRIANGLES, 36, 6);

		// Draw cube reflection
		glStencilFunc(GL_EQUAL, 1, 0xFF);
		glStencilMask(0x00);
		glDepthMask(GL_TRUE);

		model = glm::scale(glm::translate(model, glm::vec3(0, 0, -1)), glm::vec3(1, 1, -1));
		glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));

		glUniform3f(uniColor, 0.3f, 0.3f, 0.3f);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glUniform3f(uniColor, 1.0f, 1.0f, 1.0f);

		glDisable(GL_STENCIL_TEST);

		// Bind default framebuffer and draw contents of our framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindVertexArray(vaoQuad);
		glDisable(GL_DEPTH_TEST);
		glUseProgram(screenProgram);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texColorBuffer);

		glDrawArrays(GL_TRIANGLES, 0, 6);


		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

	glDeleteRenderbuffers(1, &rboDepthStencil);
	glDeleteTextures(1, &texColorBuffer);
	glDeleteFramebuffers(1, &frameBuffer);

	glDeleteTextures(1, &tex);

	glDeleteProgram(screenProgram);
	glDeleteProgram(sceneProgram);

	glDeleteBuffers(1, &vboCube);
	glDeleteBuffers(1, &vboQuad);

	glDeleteVertexArrays(1, &vaoCube);
	glDeleteVertexArrays(1, &vaoQuad);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}