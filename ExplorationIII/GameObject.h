#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "Model.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <string>
#include <btBulletDynamicsCommon.h>
#include <BulletCollision/Gimpact/btGImpactCollisionAlgorithm.h>
#include "BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h"

class GameObject{
public:
	GameObject(char* meshName, glm::vec3 position = glm::vec3(0, 0, 0));
	void Update();
	void Render();
	void SetPosition(glm::vec3);
	void Translate(glm::vec3);
	void SetRotation(glm::vec3);
	void Rotate(glm::vec3);
	void SetScale(glm::vec3);
	int GetID();
	btRigidBody* GetBody();
	static int count;
private:
	Model model;
	int ID;
	glm::vec3 scale;
	glm::vec3 position;
	glm::vec3 rotation;
	glm::mat4 modelMatrix;
	//Physics
	btCollisionShape* collisionShape;
	btRigidBody* body;
};

#endif