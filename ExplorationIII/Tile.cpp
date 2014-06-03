#include "Tile.h"
#include "Model.h"

GLuint elements[] = { 0, 1, 2, 2, 3, 0 };
Shader* Tile::shader;
GLuint Tile::vbo;
GLuint Tile::vao;
GLuint Tile::ebo;
int Tile::indicesCount;
int* Tile::indices;
int Tile::count;
float* Tile::vertFloats;
bool Tile::debug;

 void Tile::Initialize(glm::vec2 size){
	 debug = true;
	 vertFloats = (float*)malloc(sizeof(float)* 8 * 4* (size.x * size.y));
	 indices = (int*)malloc(sizeof(int)* 6 * (size.x * size.y));
 }
 void Tile::Finalize(){
	 // Create Vertex Array Object
	 glGenVertexArrays(1, &vao);
	 glBindVertexArray(vao);

	 // Create a Vertex Buffer Object and copy the vertex data to it
	 glGenBuffers(1, &vbo);
	 glBindBuffer(GL_ARRAY_BUFFER, vbo);
	 glBufferData(GL_ARRAY_BUFFER, sizeof(float) * count, vertFloats, GL_STATIC_DRAW);

	 glEnableVertexAttribArray(0);
	 glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float)* 8, 0);
	 glEnableVertexAttribArray(1);
	 glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float)* 8, (void*)(sizeof(float)* 3));
	 glEnableVertexAttribArray(2);
	 glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(float)* 8, (void*)(sizeof(float)* 5));

	 // Create an element array
	 glGenBuffers(1, &ebo);
	 glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	 glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * indicesCount, indices, GL_STATIC_DRAW);

	 glBindVertexArray(0);
 }

Tile::Tile(){

}
void Tile::Create(glm::vec2 coordinates, glm::vec3 verts2[4]){
	lowestPoint = verts2[0].z;
	for (int x = 0; x < 4; x++){
		if (verts2[x].z < lowestPoint)
			lowestPoint = verts2[x].z;
		this->verts[x] = verts2[x];
		this->verts[x].x += 20.f;
		this->verts[x].z += 20.f;
		this->verts[x].y -= 10.f;
	}

	int startCount = count;
	float offset = 10.0;
	float inlet = 0.1;

	position = glm::vec3(verts2[0].x + ((verts2[1].x - verts2[0].x) / 2.0f) - inlet, lowestPoint, verts2[0].z + ((verts2[2].z - verts2[0].z)/2.0f) - inlet);


	vertFloats[count++] = (float)(verts[0].x + inlet);
	vertFloats[count++] = (float)(verts[0].y + 0.1);
	vertFloats[count++] = (float)(verts[0].z + inlet);
	vertFloats[count++] = (float)(0.0);
	vertFloats[count++] = (float)(0.0);
	vertFloats[count++] = (float)(0.0);
	vertFloats[count++] = (float)(0.0);
	vertFloats[count++] = (float)(0.0);

	vertFloats[count++] = (float)(verts[1].x - inlet);
	vertFloats[count++] = (float)(verts[1].y + 0.1);
	vertFloats[count++] = (float)(verts[1].z + inlet);
	vertFloats[count++] = (float)(0.0);
	vertFloats[count++] = (float)(0.0);
	vertFloats[count++] = (float)(0.0);
	vertFloats[count++] = (float)(0.0);
	vertFloats[count++] = (float)(0.0);

	vertFloats[count++] = (float)(verts[2].x - inlet);
	vertFloats[count++] = (float)(verts[2].y + 0.1);
	vertFloats[count++] = (float)(verts[2].z - inlet);
	vertFloats[count++] = (float)(0.0);
	vertFloats[count++] = (float)(0.0);
	vertFloats[count++] = (float)(0.0);
	vertFloats[count++] = (float)(0.0);
	vertFloats[count++] = (float)(0.0);

	vertFloats[count++] = (float)(verts[3].x + inlet);
	vertFloats[count++] = (float)(verts[3].y + 0.1);
	vertFloats[count++] = (float)(verts[3].z - inlet);
	vertFloats[count++] = (float)(0.0);
	vertFloats[count++] = (float)(0.0);
	vertFloats[count++] = (float)(0.0);
	vertFloats[count++] = (float)(0.0);
	vertFloats[count++] = (float)(0.0);

	for (int x = 0; x < 6; x++){
		indices[indicesCount++] = (startCount/8) + elements[x];
	}

	this->coordinates = coordinates;
}
glm::vec3 Tile::getPosition(){
	return position;
}
void Tile::Render(){
	if (debug == true){
		shader->Use();
		shader->SetUniform("overrideColor", glm::vec4(1.0, 0.0, 0.0, 1.0));
		shader->SetUniform("doOverride", 1);
		glDisable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		glEnable(GL_CULL_FACE);
		shader->SetUniform("overrideColor", glm::vec4(0.0, 0.0, 0.0, 0.0));
		shader->SetUniform("doOverride", 0);
	}
}