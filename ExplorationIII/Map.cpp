#include "Map.h"
#include "Model.h"

Map::Map(glm::vec2 size, char* terrainPath, glm::vec3 terrainSize){
	tileSize = glm::vec2(1.0, 1.0);
	mapSize = glm::vec2(terrainSize.x/2, terrainSize.z/2);
	terrain = new Terrain();
	terrain->SetRenderSize(terrainSize);
	std::vector< std::vector<Tile*>> tempTiles(mapSize.x, std::vector<Tile*>(mapSize.y));
	tiles = tempTiles;
	for (int i = 0; i< mapSize.x - 1; i++) {
		for (int j = 0; j< mapSize.y - 1; j++) {
			tiles[j][i] = new Tile();
		}
	};
	terrain->LoadHeightMap(terrainPath, tiles);
	Tile::Finalize();
};
glm::vec3 Map::getTilePosition(glm::vec2 pos){
	return tiles[pos.x][pos.y]->getPosition();
};
void Map::Update(){
	terrain->Update();
};
void Map::Render(){
	terrain->Render();
	if (debug){
		Tile::shader->Use();
		Tile::shader->SetModelAndNormalMatrix("modelMatrix", "normalMatrix", glm::mat4(1.0f));
		Tile::Render();
	}
};
void Map::SwapDebug(){
	debug = !debug;
}
void Map::SetTileSize(glm::vec2 size){
	tileSize = size;
};
Terrain* Map::getTerrain(){
	return terrain;
}