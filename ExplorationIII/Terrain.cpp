#include "Terrain.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"

Shader* Terrain::shader = NULL;
Texture Terrain::textures[5];

bool Terrain::shadersLoaded;

Terrain::Terrain(){
	
}

void Terrain::LoadTextures(){
	std::string textureNames[] = { "fungus.dds", "sand_grass_02.jpg", "rock_2_4w.jpg", "sand.jpg", "path.png" };
	for (int x = 0; x < 5; x++){
		textures[x].Load("data\\textures\\" + textureNames[x], true);
	}
}

void Terrain::Render(){
	shader->Use();
	shader->SetUniform("fRenderHeight", renderScale.y);
	shader->SetUniform("fMaxTextureU", float(columns)*0.1f);
	shader->SetUniform("fMaxTextureV", float(rows)*0.1f);
	shader->SetUniform("HeightmapScaleMatrix", glm::scale(glm::mat4(1.0), glm::vec3(renderScale)));
	shader->SetUniform("ambientIntensity", 0.2f);
	GLuint sampler = textures[0].samplerID;
	for (int x = 0; x < 5; x++){
		char sSamplerName[256];
		sprintf(sSamplerName, "gSampler[%d]", x);
		shader->SetUniform(sSamplerName, x);
		textures[x].Bind(x, sampler);
	}
	glBindVertexArray(vaoID);
	int iNumIndices = (rows - 1)*columns * 2 + rows - 1;//((400 * 199) + 199)
	glDrawElements(GL_TRIANGLE_STRIP, iNumIndices, GL_UNSIGNED_INT,0);
	glBindVertexArray(0);

};

void Terrain::SetMatricies(){
	glm::mat4 mModelView = glm::mat4(1.0f);// glm::rotate(glm::mat4(1.0f), 90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	mModelView = glm::translate(mModelView,glm::vec3(20.0f, -10.f, 20.0f));
	shader->SetModelAndNormalMatrix("modelMatrix", "normalMatrix", mModelView);
};

bool Terrain::LoadHeightMap(std::string image, std::vector<std::vector<Tile*>> tiles){
	FIBITMAP* dib = LoadHeightMapImage(image);
	BYTE* bDataPointer = FreeImage_GetBits(dib);
	rows = FreeImage_GetHeight(dib);
	columns = FreeImage_GetWidth(dib);

	if (bDataPointer == NULL || rows == 0 || columns == 0 || (FreeImage_GetBPP(dib) != 24 && FreeImage_GetBPP(dib) != 8))
		return false;

	unsigned int ptr_inc = FreeImage_GetBPP(dib) == 24 ? 3 : 1;
	unsigned int row_step = ptr_inc*columns;

	std::vector< std::vector< glm::vec3> > vVertexData(rows, std::vector<glm::vec3>(columns));
	std::vector< std::vector< glm::vec3> > vCoordsData(rows, std::vector<glm::vec3>(columns));

	float fTextureU = float(columns)*0.1f;
	float fTextureV = float(rows)*0.1f;

	physicsVerts = new btVector3[columns*rows];
	glm::vec3* mapVerts = new glm::vec3[(columns) * (rows)];
	int count3 = 0;
	int count2 = 0;
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < columns; j++) {
			float fScaleC = float(j) / float(columns - 1);
			float fScaleR = float(i) / float(rows - 1);
			float fVertexHeight = float(*(bDataPointer + row_step*i + j*ptr_inc)) / 255.0f;
			if (j % 2 == 0 && i % 2 == 0){
				mapVerts[count3++] = glm::vec3(renderScale.x * (-0.5f + fScaleC), renderScale.y*(fVertexHeight), renderScale.z*(-0.5f + fScaleR));
				physicsVerts[count2++].setValue(renderScale.x * (-0.5f + fScaleC), renderScale.y*(fVertexHeight), renderScale.z*(-0.5f + fScaleR));
			}
			//Create a new tile
			vVertexData[i][j] = glm::vec3(-0.5f + fScaleC, fVertexHeight, -0.5f + fScaleR);
			vCoordsData[i][j] = glm::vec3(fTextureU*fScaleC, fTextureV*fScaleR, 0.0);
		}
	}
	std::vector< std::vector<glm::vec3> > vNormals[2];
	for (int i = 0; i < 2; i++)
		vNormals[i] = std::vector< std::vector<glm::vec3> >(rows - 1, std::vector<glm::vec3>(columns - 1));

	for(int i = 0; i < (rows-1); i++) {
		for(int j = 0; j < columns-1; j++) {
			glm::vec3 vTriangle0[] = { vVertexData[i][j], vVertexData[i + 1][j], vVertexData[i + 1][j + 1] };
			glm::vec3 vTriangle1[] = { vVertexData[i + 1][j + 1], vVertexData[i][j + 1], vVertexData[i][j] };

			glm::vec3 vTriangleNorm0 = glm::cross(vTriangle0[0] - vTriangle0[1], vTriangle0[1] - vTriangle0[2]);
			glm::vec3 vTriangleNorm1 = glm::cross(vTriangle1[0] - vTriangle1[1], vTriangle1[1] - vTriangle1[2]);

			vNormals[0][i][j] = glm::normalize(vTriangleNorm0);
			vNormals[1][i][j] = glm::normalize(vTriangleNorm1);
		}
	}

	std::vector< std::vector<glm::vec3> > vFinalNormals = CalculateFinalNormal(vNormals);

	data.Create(rows*columns * 8);
	std::vector< std::vector<glm::vec3> > info[3] = { vVertexData, vCoordsData, vFinalNormals };
	data.Add(info, glm::vec2(rows, columns));

	indicesArray = (int*)malloc(sizeof(int)* (((2 * rows) * (columns - 1)) + (columns - 1)));
	int count = 0;
	count2 = 0;
	int iPrimitiveRestartIndex = rows*columns;
	int* physicsIndices = (int*)malloc(sizeof(int)* (2 * rows * (rows - 1)));
	for (int i = 0; i < (rows - 1); i++){
		for (int j = 0; j < columns; j++){
			for (int k = 0; k < 2; k++){
				int iRow = i + (0 + k);
				int iIndex = iRow*columns + j;
				indicesArray[count++] = iIndex;
				physicsIndices[count2++] = iIndex;
			}
		}
		indicesArray[count++] = iPrimitiveRestartIndex;
	}
	Tile::Initialize(glm::vec2(rows/2,columns/2));
	for (int i = 0; i< (rows/2) - 1; i++) {
		for (int j = 0; j < (columns/2) - 1; j++) {
				glm::vec3 tempVertsArray[] = { mapVerts[i*(columns / 2) + j], mapVerts[i*(columns / 2) + j + 1], mapVerts[(i + 1)*(columns / 2) + j + 1], mapVerts[(i + 1)*(columns / 2) + j] };
				//glm::vec3 tempVertsArray[] = { mapVerts[j*(columns / 2) + i], mapVerts[j*(columns / 2) + i + 1], mapVerts[(j + 1)*(columns / 2) + i + 1], mapVerts[(j + 1)*(columns / 2) + 1] };
				tiles[j][i]->Create(glm::vec2(j, i), tempVertsArray);
		}
	};
	CreateTerrainBody();
	FinalizeTerrain();
}
void Terrain::CreateTerrainBody(){
	//Physics
	heightScale = renderScale.y;
	btTriangleMesh* trimesh = new btTriangleMesh();
	int NUM_VERTS_X = columns / 2;
	int NUM_VERTS_Y = rows / 2;
	for (int i = 0; i<NUM_VERTS_X - 1; i++) {
		for (int j = 0; j<NUM_VERTS_Y - 1; j++) {
			trimesh->addTriangle(physicsVerts[j*NUM_VERTS_X + i], physicsVerts[j*NUM_VERTS_X + i + 1], physicsVerts[(j + 1)*NUM_VERTS_X + i + 1]);
			trimesh->addTriangle(physicsVerts[j*NUM_VERTS_X + i], physicsVerts[(j + 1)*NUM_VERTS_X + i + 1], physicsVerts[(j + 1)*NUM_VERTS_X + i]);
		}
	}
	delete[] physicsVerts;
	terrainMesh = new btBvhTriangleMeshShape(trimesh, true);
	transform.setIdentity();
	btVector3 inertia(0, 0, 0);
	myMotionState = new btDefaultMotionState(transform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(0.0f, myMotionState, terrainMesh);
	body = new btRigidBody(rbInfo);
	body->setFriction(0.8f);
	body->setHitFraction(0.8f);
	body->setRestitution(0.6f);
	body->translate(btVector3(20.f, -9.8f, 20.f));
};

void Terrain::SetRenderSize(float fQuadSize, float fHeight){
	renderScale = glm::vec3(float(columns)*fQuadSize, fHeight, float(rows)*fQuadSize);
}

void Terrain::SetRenderSize(glm::vec3 size){
	renderScale = size;
}

int Terrain::GetRowCount(){
	return rows;
};

int Terrain::GetColumnCount(){
	return columns;
};

void Terrain::Update(){
	shader->Use();
	SetMatricies();
}
btRigidBody* Terrain::GetBody(){
	return body;
}
FIBITMAP* Terrain::LoadHeightMapImage(std::string image){
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	FIBITMAP* dib(0);
	fif = FreeImage_GetFileType(image.c_str(), 0);
	if (fif == FIF_UNKNOWN)
		fif = FreeImage_GetFIFFromFilename(image.c_str());
	if (fif == FIF_UNKNOWN)
		return NULL;
	if (FreeImage_FIFSupportsReading(fif))
		dib = FreeImage_Load(fif, image.c_str());
	if (!dib)
		return NULL;
	return dib;
}
std::vector< std::vector<glm::vec3> > Terrain::CalculateFinalNormal(std::vector< std::vector<glm::vec3> > vNormals[2]) {
	std::vector< std::vector<glm::vec3> > vFinalNormals = std::vector< std::vector<glm::vec3> >(rows, std::vector<glm::vec3>(columns));
	for (int i = 0; i < rows; i++){
		for (int j = 0; j < columns; j++){
			glm::vec3 vFinalNormal = glm::vec3(0.0f, 0.0f, 0.0f);
			if (j != 0 && i != 0)
				for (int k = 0; k < 2; k++) vFinalNormal += vNormals[k][i - 1][j - 1];
			if (i != 0 && j != columns - 1)vFinalNormal += vNormals[0][i - 1][j];
			if (i != rows - 1 && j != columns - 1)
				for (int k = 0; k < 2; k++) vFinalNormal += vNormals[k][i][j];
			if (i != rows - 1 && j != 0)
				vFinalNormal += vNormals[1][i][j - 1];
			vFinalNormal = glm::normalize(vFinalNormal);

			vFinalNormals[i][j] = vFinalNormal;
		}
	}
	return vFinalNormals;
};
bool Terrain::FinalizeTerrain(){
	int iPrimitiveRestartIndex = rows*columns;
	GLuint indicesID;
	glGenVertexArrays(1, &vaoID);
	glBindVertexArray(vaoID);
	data.BindCreate();
	data.Upload(GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float)* 8, 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float)* 8, (void*)(sizeof(float)* 3));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(float)* 8, (void*)(sizeof(float)* 5));
	glGenBuffers(1, &indicesID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int)* (2 * rows * (columns - 1) + (columns - 1)), indicesArray, GL_STATIC_DRAW);
	glEnable(GL_PRIMITIVE_RESTART);
	glPrimitiveRestartIndex(iPrimitiveRestartIndex);
	glBindVertexArray(0);
	LoadTextures();
	isLoaded = true;
	return true;
};