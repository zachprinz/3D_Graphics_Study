#ifndef SHADER_H
#define SHADER_H

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <iostream>

using namespace std;

class Shader{
public:
	Shader(const char * vertex_file_path, const char * fragment_file_path);
	void Use();
	GLuint ID;
	bool isLinked;

	void SetUniform(string sName, const glm::vec3 vVector);
	void SetUniform(string sName, const glm::vec4 vVector);
	void SetUniform(string sName, const float fValue);
	void SetUniform(string sName, const glm::mat4 mMatrix);
	void SetUniform(string sName, const int);
	void SetModelAndNormalMatrix(string sModelMatrixName, string sNormalMatrixName, glm::mat4 mModelMatrix);
};

#endif