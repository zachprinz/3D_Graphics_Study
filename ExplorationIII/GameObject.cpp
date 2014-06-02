#include "GameObject.h"
#include "Scene.h"

#define X_AXIS glm::vec3(1.f,0.f,0.f)
#define Y_AXIS glm::vec3(0.f,1.f,0.f)
#define Z_AXIS glm::vec3(0.f,0.f,1.f)

int GameObject::count;

GameObject::GameObject(char* meshName, glm::vec3 position){
	ID = count++;
	model.Load(meshName);
	this->position = position;
	rotation = glm::vec3(0.0, 0.0, 0.0);
	scale = glm::vec3(1.0, 1.0, 1.0);
	//Physics
	collisionShape = new btBoxShape(btVector3(1.0, 1.0, 1.0));
	btVector3 bodyPosition(position.x, position.y +20, position.z);
	btQuaternion orientation(1.0, 0.0, 0.0, 0.0);
	btTransform transform(orientation, bodyPosition);
	btDefaultMotionState* motionstate = new btDefaultMotionState(transform);
	btRigidBody::btRigidBodyConstructionInfo bodyInfo(1, motionstate, collisionShape, btVector3(0, 0, 0));
	body = new btRigidBody(bodyInfo);
	Scene::Instance->AddObject(this);
};
void GameObject::Update(){
	btTransform trans;
	body->getMotionState()->getWorldTransform(trans);
	position = glm::vec3(trans.getOrigin().getX(), trans.getOrigin().getY() - 1.0, trans.getOrigin().getZ());
	modelMatrix = glm::scale(glm::mat4(1.0), scale);
	modelMatrix = glm::rotate(modelMatrix, rotation.x, X_AXIS);
	modelMatrix = glm::rotate(modelMatrix, rotation.y, Y_AXIS);
	modelMatrix = glm::rotate(modelMatrix, rotation.z, Z_AXIS);
	modelMatrix = glm::translate(modelMatrix, position);
};
void GameObject::Render(){
	model.Update(modelMatrix);
	model.Render();
};
void GameObject::SetPosition(glm::vec3 position){
	this->position = position;
};
void GameObject::Translate(glm::vec3 translation){
	this->position += translation;
	body->translate(btVector3(translation.x, translation.y, translation.z));
};
void GameObject::SetRotation(glm::vec3 rotation){
	this->rotation = rotation;
};
void GameObject::Rotate(glm::vec3 rotation){
	this->rotation += rotation;
};
void GameObject::SetScale(glm::vec3 scale){
	this->scale = scale;
};
int GameObject::GetID(){
	return ID;
};
btRigidBody* GameObject::GetBody(){
	return body;
}
