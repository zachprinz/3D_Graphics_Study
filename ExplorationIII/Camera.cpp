#include "Camera.h"
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include "GameObject.h"
#include <string>

#define X_AXIS glm::vec3(1.f,0.f,0.f)
#define Y_AXIS glm::vec3(0.f,1.f,0.f)
#define Z_AXIS glm::vec3(0.f,0.f,1.f)
#define MARGIN 10

GLuint Camera::viewMatrixID;
GLuint Camera::simpleViewMatrixID;
GLuint Camera::simpleID;
GLuint Camera::shaderID;
glm::vec3 Camera::position;
Camera* Camera::Instance = NULL;

Camera::Camera(){
	if (Instance == NULL)
		Instance = this;
	proj = glm::perspective(45.0f, 800.0f / 600.0f, 1.0f, 500.0f);
	position = glm::vec3(0.0f,12.0f, 23.0f);
	focus = glm::vec3(position.x, position.y + 2.0f, position.z - 1);// ::vec3(0.0f, 2.0f, 3.0f);
	up = glm::vec3(0.f, 1.f, 0.f);
	xAngle = 0.f;
	yAngle = 0.f;
}

void Camera::Translate(glm::vec3 inVec){
	position += inVec;
}

void Camera::SetAngle(float x, float y){
	xAngle += x / 20.f;
	yAngle += y / 20.f;

	glm::vec3 View(1.0f, 0.0f, 0.0f);
	View = glm::rotate(View, xAngle, Y_AXIS);
	View = glm::normalize(View);

	glm::vec3 hAxis = glm::cross(Y_AXIS, View);
	hAxis = glm::normalize(hAxis);
	View = glm::rotate(View, yAngle, hAxis);
	View = glm::normalize(View);

	focus = View;
	focus = glm::normalize(focus);

	up = glm::cross(focus, hAxis);
	up = glm::normalize(up);
}

void Camera::Update(){
	position = target->GetPosition();
	position.z += 30.0f;
	position.y += 30.0f;
	focus = glm::vec3(target->GetPosition().x, target->GetPosition().y - 1.0, target->GetPosition().z - 5.0);
	view = glm::lookAt(position, focus, up);
	for (int x = 0; x < shaders.size(); x++){
		shaders.at(x)->Use();
		shaders.at(x)->SetUniform("projMatrix", proj);
		shaders.at(x)->SetUniform("viewMatrix", view);
		shaders.at(x)->SetUniform("lightDirection", glm::vec3(1.0f, 1.0f, 1.0f));
	}
}

glm::vec3 Camera::GetPosition(){
	return position;
}
void Camera::SetTarget(GameObject* newTarget){
	target = newTarget;
}