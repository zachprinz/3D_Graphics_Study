#ifndef TERRAIN_H
#define TERRAIN_H

#include <iostream>
#include <string>
#include <GL/glew.h>
#include "VBO.h"
#include "Texture.h"
#include "Shader.h"
#include <btBulletDynamicsCommon.h>
#include <BulletCollision/Gimpact/btGImpactCollisionAlgorithm.h>
#include "BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h"
#include "BulletCollision/CollisionShapes/btBvhTriangleMeshShape.h"
#include "Tile.h"


#define NUMSHADERS 4

class Terrain{
public:
	static bool LoadTerrainShaders();
	bool LoadHeightMap(std::string image, std::vector<std::vector<Tile*>> tiles);
	void Render();
	void Update();
	void SetRenderSize(float fQuadSize, float fHeight);
	void SetRenderSize(glm::vec3);
	int GetRowCount();
	int GetColumnCount();
	void SetMatricies();
	static void LoadTextures();
	Terrain();
	static Shader* shader;
	int* indicesArray;
	btRigidBody* GetBody();
private:
	void CreateTerrainBody();
	bool FinalizeTerrain();
	FIBITMAP* LoadHeightMapImage(std::string);
	std::vector< std::vector<glm::vec3> > CalculateFinalNormal(std::vector< std::vector<glm::vec3> > vNormals[2]);
	btVector3* physicsVerts;
	GLuint vaoID;
	bool isLoaded;
	static bool shadersLoaded;
	int rows, columns, iRows, iColumns;
	glm::vec3 renderScale;
	VBO data;
	VBO indices;
	static Texture textures[5];
	static GLuint terrainShaders[NUMSHADERS];
	btBvhTriangleMeshShape* terrainMesh;
	btTransform transform;
	btDefaultMotionState* myMotionState;
	btRigidBody* body;
	float heightScale;
};

#endif
