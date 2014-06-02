#ifndef VBO_H
#define VBO_H

#include <GL/glew.h>
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <algorithm>
#include <cmath>
#include <cstring>
#include <sstream>
#include <queue>
#include <map>
#include <set>
#include <gl/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <FI/FreeImage.h>

class VBO{
public:
	VBO();
	void Create(int size);
	void Delete();
	void* MapBufferToMemory(int usage);
	void* MapSubBufferToMemory(int usage, GLuint offset, GLuint length);
	void Unmap();
	void Bind(int type = GL_ARRAY_BUFFER);
	void BindCreate(int type = GL_ARRAY_BUFFER);
	void Upload(GLuint usage = GL_STATIC_DRAW);
	void AddData(float*);
	void* GetDataPointer();
	GLuint GetID();
	int GetSize();
	int count;
	int debugCount;
	float* data;
	void Add(std::vector< std::vector< glm::vec3> > data[3], glm::vec2 size);
private:
	GLuint id;
	int size, currentSize, type;
	bool dataUploaded;
};

#endif