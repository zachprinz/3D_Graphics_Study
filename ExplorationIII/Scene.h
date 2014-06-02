#ifndef SCENE_H
#define SCENE_H

#include "Skybox.h"
#include "GameObject.h"
#include "Map.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <btBulletDynamicsCommon.h>
#include <BulletCollision/Gimpact/btGImpactCollisionAlgorithm.h>
#include "GLDebugDrawer.h"
#include <BulletCollision/CollisionDispatch/btGhostObject.h>

class Scene{
public:
	Scene(char* terrainPath, char* skyboxPath, glm::vec3);
	void Update();
	void Render();
	void AddObject(GameObject*);
	static Scene* Instance;
	btAlignedObjectArray<btCollisionShape*> shapes;
	btDiscreteDynamicsWorld*			 dynamicsWorld;
	Map* getMap();
private:
	void UpdateObjects();
	void RenderObjects();
	std::vector<GameObject*> objects;
	Skybox* skybox;
	Map* map;
	//Physics
	btBroadphaseInterface*				 broadphase;
	btDefaultCollisionConfiguration*     collisionConfiguration;
	btCollisionDispatcher*				 dispatcher;
	btSequentialImpulseConstraintSolver* solver;
	GLDebugDrawer						 debugDrawer;
	btAxisSweep3 *						 m_overlappingPairCache;
	btOverlappingPairCallback*			 m_ghostPairCallback;
	float currentTime;
	float lastTime;
};

#endif