#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <stdlib.h>
#include <string.h>
#include <GL/glew.h>
#include "shader.h"

using namespace std;

Shader::Shader(const char * vertex_file_path, const char * fragment_file_path){

	//Create the initial ID's for the shaders.
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	//Read in the vertex shader code as a char*
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if (VertexShaderStream.is_open()){
		std::string Line = "";
		while (getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}
	else{
		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
		getchar();
	}

	//Read in the fragment shader code as a char*
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if (FragmentShaderStream.is_open()){
		std::string Line = "";
		while (getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	//Set the source of the vertex shader, then compile it.
	printf("Compiling shader : %s\n", vertex_file_path);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
	glCompileShader(VertexShaderID);

	//Check the results of compiling the vertex shader.
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0){
		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}


	//Add the code to the fragment shader then compile it.
	printf("Compiling shader : %s\n", fragment_file_path);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	glCompileShader(FragmentShaderID);

	//Check the fragment shader for errors. See if everything went right.
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0){
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}

	//Creat a program and then link the shaders with the shader program.
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0){
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);
	glBindFragDataLocation(ProgramID, 1, "outputColor");
	glLinkProgram(ProgramID);
	isLinked = true;
	ID = ProgramID;
}

void Shader::SetModelAndNormalMatrix(string sModelMatrixName, string sNormalMatrixName, glm::mat4 mModelMatrix){
	SetUniform(sModelMatrixName, mModelMatrix);
	SetUniform(sNormalMatrixName, glm::transpose(glm::inverse(mModelMatrix)));
}

void Shader::SetUniform(string sName, const glm::mat4 mMatrix){
	int iLoc = glGetUniformLocation(ID, sName.c_str());
	glUniformMatrix4fv(iLoc, 1, GL_FALSE, (GLfloat*)&mMatrix);
}

void Shader::SetUniform(string sName, const glm::vec3 vVector){
	int iLoc = glGetUniformLocation(ID, sName.c_str());
	glUniform3fv(iLoc, 1, (GLfloat*)&vVector);
}

void Shader::SetUniform(string sName, const glm::vec4 vVector){
	int iLoc = glGetUniformLocation(ID, sName.c_str());
	glUniform4fv(iLoc, 1, (GLfloat*)&vVector);
}

void Shader::SetUniform(string sName, const float fValue){
	int iLoc = glGetUniformLocation(ID, sName.c_str());
	glUniform1fv(iLoc, 1, &fValue);
}
void Shader::SetUniform(string sName, const int iValue){
	int iLoc = glGetUniformLocation(ID, sName.c_str());
	glUniform1i(iLoc, iValue);
}

void Shader::Use(){
	if (isLinked)glUseProgram(ID);
}
