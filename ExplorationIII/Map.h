#ifndef MAP_H
#define MAP_H

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Terrain.h"
#include "Tile.h"

class Map{
public:
	Map(glm::vec2 size, char* terrainPath, glm::vec3 terrainSize);
	std::vector<std::vector<Tile*>> tiles;
	glm::vec3 getTilePosition(glm::vec2);
	void Update();
	void Render();
	void SetTileSize(glm::vec2);
	Terrain* getTerrain();
private:
	glm::vec2 mapSize;
	glm::vec2 tileSize;
	glm::vec3 terrainSize;
	Terrain* terrain;
};

#endif