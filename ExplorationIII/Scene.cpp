#include "Scene.h"
#include "Camera.h"
#include "Actor.h"

Scene* Scene::Instance = NULL;

#define BIT(x) (1<<(x))
#define COL_NOTHING 0
#define COL_OBJECT BIT(0)
#define COL_WALL BIT(1)

#define OBJECT_COLLISION_MASK COL_WALL
#define WALL_COLLISION_MASK COL_NOTHING

Scene::Scene(char* terrainPath, char* skyboxPath, glm::vec3 terrainSize){
	if (Instance == NULL) Instance = this;
	map = new Map(glm::vec2(100, 100), terrainPath, terrainSize);
	skybox = new Skybox();
	skybox->Load(skyboxPath, "front.jpg", "back.jpg", "left.jpg", "right.jpg", "top.jpg", "bottom.jpg");

	//Physics
	broadphase = new btDbvtBroadphase();
	m_ghostPairCallback = new btGhostPairCallback();
	broadphase->getOverlappingPairCache()->setInternalGhostPairCallback(m_ghostPairCallback);
	btVector3 worldMin(-1000, -1000, -1000);
	btVector3 worldMax(1000, 1000, 1000);
	m_overlappingPairCache = new btAxisSweep3(worldMin, worldMax);
	collisionConfiguration = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collisionConfiguration);
	btGImpactCollisionAlgorithm::registerAlgorithm(dispatcher);
	solver = new btSequentialImpulseConstraintSolver;
	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, m_overlappingPairCache, solver, collisionConfiguration);//m_overlappingPairCache
	m_overlappingPairCache->getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback()); // This maybe should be in actor?
	dynamicsWorld->setGravity(btVector3(0, -10, 0));
	debugDrawer.setDebugMode(0);
	dynamicsWorld->setDebugDrawer(&debugDrawer);
	dynamicsWorld->addRigidBody(map->getTerrain()->GetBody());
	shapes.push_back(map->getTerrain()->GetBody()->getCollisionShape());
	double lastTime = glfwGetTime();
};
void Scene::Update(){
	currentTime = glfwGetTime();
	float elapsedTime = currentTime - lastTime;
	lastTime = currentTime;
	dynamicsWorld->stepSimulation(elapsedTime, 7);
	skybox->Update();
	map->Update();
	UpdateObjects();
};
void Scene::Render(){
	skybox->Render();
	map->Render();
	RenderObjects();
	if (debug){
		debugDrawer.SetMatrices(Camera::Instance->view, Camera::Instance->proj);
		dynamicsWorld->debugDrawWorld();
	}
};
void Scene::UpdateObjects(){
	for (int x = 0; x < objects.size(); x++)
		objects.at(x)->Update();
};
void Scene::RenderObjects(){
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_CULL_FACE);
	for (int x = 0; x < objects.size(); x++)
		objects.at(x)->Render();
};
void Scene::AddObject(GameObject* object){
	objects.push_back(object);
	dynamicsWorld->addRigidBody(object->GetBody());// , COL_OBJECT, OBJECT_COLLISION_MASK);
	shapes.push_back(object->GetBody()->getCollisionShape());
};
void Scene::AddActor(Actor* actor){
	objects.push_back(actor);
	dynamicsWorld->addCollisionObject(actor->GetGhostObject(), btBroadphaseProxy::CharacterFilter, btBroadphaseProxy::StaticFilter | btBroadphaseProxy::DefaultFilter);
	dynamicsWorld->addAction(actor->GetCharacter());
}
Map* Scene::getMap(){
	return map;
}
void Scene::SwapDebug(){
	debug = !debug;
	if (debug)
		debugDrawer.setDebugMode(1);
	else
		debugDrawer.setDebugMode(0);
}