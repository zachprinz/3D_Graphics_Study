#ifndef CAMERA_H
#define CAMERA_H

#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include "Shader.h"
#include "GameObject.h"

class Camera{
public:
	Camera();
	void Translate(glm::vec3);
	void SetAngle(float, float);
	void Update();
	glm::vec3 GetPosition();
	static glm::vec3 position;
	glm::mat4 proj;
	glm::mat4 view;
	std::vector<Shader*> shaders;
	static Camera* Instance;
	void SetTarget(GameObject*);
private:
	GameObject* target;
	float xAngle;
	float yAngle;
	float xDelta;
	float yDelta;
	float mousePositionX;
	float mousePositionY;
	glm::vec3 focus;
	glm::vec3 up;
	static GLuint viewMatrixID;
	static GLuint simpleViewMatrixID;
	static GLuint simpleID, shaderID;
	bool mouseLeft, mouseRight, mouseUp, mouseDown;
};

#endif