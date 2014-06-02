#ifndef TILE_H
#define TILE_H

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>

class Tile{
public:
	Tile();
	static void Render();
	void Create(glm::vec2 coordinates, glm::vec3 verts2[4]);
	glm::vec3 getPosition();
	static void Finalize();
	static void Initialize(glm::vec2);
	static int* indices;
	static int indicesCount;
	static bool debug;
private:
	static int count;
	static GLuint vbo;
	static GLuint vao;
	static GLuint ebo;
	static float* vertFloats;
	float lowestPoint;
	glm::vec3 position;
	glm::vec3 verts[4];
	glm::vec2 coordinates;
};

#endif