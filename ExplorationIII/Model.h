#ifndef MODEL_H
#define MODEL_H

#include "VBO.h"
#include "Material.h"
#include "Shader.h"
#include "Texture.h"
#include <algorithm>
#include <cmath>
#include <cstring>
#include <vector>
#include <sstream>
#include <queue>
#include <map>
#include <set>
#include <vector>
#include <iostream>
#include <gl/glew.h>
#include <glm/glm.hpp>
#include "Shader.h"

#include <FI/FreeImage.h>


class Model{
public:
	Model();
	bool Load(char* filePath);
	void FinalizeVBO();
	void BindModelsVAO();
	static float neededSize;
	void Update(glm::mat4);
	void Render();
	static Shader* shader;
private:
	bool isLoaded;
	VBO myVBO;
	GLuint VAOid;
	static std::vector<Texture> textures;
	std::vector<int> meshStartIndices;
	std::vector<int> meshSizes;
	std::vector<int> materialIndices;
	std::vector<bool> meshIsTextured;
	int numberOfMaterials;
};

#endif