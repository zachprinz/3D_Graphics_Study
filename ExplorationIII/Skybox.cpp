#include "Skybox.h"
#include "Camera.h"
#include "Model.h"

GLuint Skybox::modelID;
Shader* Skybox::shader;

Skybox::Skybox(){

}

void Skybox::Load(string path, string front, string back, string left, string right, string top, string bottom){
	std::cout << path + front << std::endl;
	textures[0].Load(path + front, true);
	textures[1].Load(path + back, true);
	textures[2].Load(path + left, true);
	textures[3].Load(path + right, true);
	textures[4].Load(path + top, true);
	textures[5].Load(path + bottom, true);

	glm::vec3 vSkyBoxVertices[24] =
	{
		glm::vec3(50.0f, 50.0f, 50.0f), glm::vec3(50.0f, -50.0f, 50.0f), glm::vec3(-50.0f, 50.0f, 50.0f), glm::vec3(-50.0f, -50.0f, 50.0f),
		glm::vec3(-50.0f, 50.0f, -50.0f), glm::vec3(-50.0f, -50.0f, -50.0f), glm::vec3(50.0f, 50.0f, -50.0f), glm::vec3(50.0f, -50.0f, -50.0f),
		glm::vec3(-50.0f, 50.0f, 50.0f), glm::vec3(-50.0f, -50.0f, 50.0f), glm::vec3(-50.0f, 50.0f, -50.0f), glm::vec3(-50.0f, -50.0f, -50.0f),
		glm::vec3(50.0f, 50.0f, -50.0f), glm::vec3(50.0f, -50.0f, -50.0f), glm::vec3(50.0f, 50.0f, 50.0f), glm::vec3(50.0f, -50.0f, 50.0f),
		glm::vec3(-50.0f, 50.0f, -50.0f), glm::vec3(50.0f, 50.0f, -50.0f), glm::vec3(-50.0f, 50.0f, 50.0f), glm::vec3(50.0f, 50.0f, 50.0f),
		glm::vec3(50.0f, -50.0f, -50.0f), glm::vec3(-50.0f, -50.0f, -50.0f), glm::vec3(50.0f, -50.0f, 50.0f), glm::vec3(-50.0f, -50.0f, 50.0f),
	};
	glm::vec2 vSkyBoxTexCoords[4] =
	{
		glm::vec2(0.0f, 1.0f), glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 0.0f)
	};

	glm::vec3 vSkyBoxNormals[6] =
	{
		glm::vec3(0.0f, 0.0f, -1.0f),
		glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(-1.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, -1.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f)
	};

	glGenVertexArrays(1, &vaoID);
	glBindVertexArray(vaoID);
	vbo.Create(192);
	vbo.BindCreate();
	for (int x = 0; x < 24; x++){
		vbo.data[vbo.count++] = vSkyBoxVertices[x].x;
		vbo.data[vbo.count++] = vSkyBoxVertices[x].y;
		vbo.data[vbo.count++] = vSkyBoxVertices[x].z;

		vbo.data[vbo.count++] = vSkyBoxTexCoords[x%4].x;
		vbo.data[vbo.count++] = vSkyBoxTexCoords[x%4].y;

		vbo.data[vbo.count++] = vSkyBoxNormals[x/4].x;
		vbo.data[vbo.count++] = vSkyBoxNormals[x/4].y;
		vbo.data[vbo.count++] = vSkyBoxNormals[x/4].z;
	}
	vbo.Upload(GL_ARRAY_BUFFER);
	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float)* 8, (void*)(sizeof(float)* 3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(float)* 8, (void*)(sizeof(float)* 5));
	glBindVertexArray(0);
}

void Skybox::Render(){
	shader->Use();
	glDepthMask(0);
	glBindVertexArray(vaoID);
	shader->SetUniform("ambientIntensity", 1.0f);
	for (int x = 0; x < 6; x++){
		textures[x].Bind(0);
		glDrawArrays(GL_TRIANGLE_STRIP, x * 4, 4);
	}
	shader->SetUniform("ambientIntensity", 0.2f);
	glBindVertexArray(0);
	glDepthMask(1);
}

void Skybox::Update(){
	shader->Use();
	shader->SetModelAndNormalMatrix("modelMatrix", "normalMatrix", glm::translate(glm::mat4(1.f), Camera::position));
}