#include "Model.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <btBulletDynamicsCommon.h>
#include <string>

std::vector<Texture> Model::textures;
Shader* Model::shader;
float Model::neededSize = 0;

std::string GetDirectoryPath(std::string sFilePath){
	// Get directory path
	std::string sDirectory = "";
	int size = sFilePath.size() - 1;
	for (int x = size; x >= 0; x--) if (sFilePath[x] == '\\' || sFilePath[x] == '/')
	{
		sDirectory = sFilePath.substr(0, x + 1);
		break;
	}
	return sDirectory;
}

Model::Model(){
	if (neededSize == 0){
		textures.reserve(50);
		neededSize = sizeof(float)*8;
		std::cout << "Needed Size: " << std::to_string(neededSize) << std::endl;
	}
	isLoaded = false;
};
bool Model::Load(char* filePath){
	Assimp::Importer importer;
	std::cout << filePath << std::endl;
	const aiScene* scene = importer.ReadFile(filePath, aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType | AI_SCENE_FLAGS_INCOMPLETE);
	//find out how many vertecies we need
	int vertexCount = 0;
	for (int x = 0; x < scene->mNumMeshes; x++){
		vertexCount += 3*scene->mMeshes[x]->mNumFaces;
	}
	std::cout << "VBO Memory: " << std::to_string(sizeof(float)* vertexCount) << std::endl;;
	myVBO.Create((sizeof(float)* vertexCount)*2);
	if (!scene){
		std::cout << "Error Loading Model" << std::endl;
		return false;
	}
	else{
		std::cout << "Loaded Model" << std::endl;
	}
	const int vertexTotalSize = neededSize;
	int totalVertices = 0;
	for (int x = 0; x < scene->mNumMeshes; x++){
		std::cout << "Working with Mesh #: " << std::to_string(x) << std::endl;
		aiMesh* mesh = scene->mMeshes[x];
		int faceCount = mesh->mNumFaces;
		materialIndices.push_back(mesh->mMaterialIndex);
		int sizeBefore = myVBO.debugCount;
		meshStartIndices.push_back(sizeBefore);
		const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);
		for (int y = 0; y < faceCount; y++){
			const aiFace& face = mesh->mFaces[y];
			for (int z = 0; z < 3; z++){
				myVBO.debugCount++;
				const aiVector3D* pos = &(mesh->mVertices[face.mIndices[z]]);
				const aiVector3D* normal = mesh->HasTextureCoords(0) ? &(mesh->mNormals[face.mIndices[z]]) : &Zero3D;
				const aiVector3D* uv = mesh->HasTextureCoords(0) ? &(mesh->mTextureCoords[0][face.mIndices[z]]) : &Zero3D;
				myVBO.data[myVBO.count++] = (pos->x);
				myVBO.data[myVBO.count++] = (pos->y);
				myVBO.data[myVBO.count++] = (pos->z);
				if (mesh->HasTextureCoords(0)){
					myVBO.data[myVBO.count++] = (mesh->mTextureCoords[0][face.mIndices[z]].x);
					myVBO.data[myVBO.count++] = (mesh->mTextureCoords[0][face.mIndices[z]].y);
				}
				else{
					//std::cout << "Failed to find tex coords" << std::endl;
					myVBO.data[myVBO.count++] = (0.0f);
					myVBO.data[myVBO.count++] = (0.0f);
				}
				if (mesh->HasNormals()){
					myVBO.data[myVBO.count++] = (normal->x);
					myVBO.data[myVBO.count++] = (normal->y);
					myVBO.data[myVBO.count++] = (normal->z);
				}
				else{
					//std::cout << "Failed to find normal coords" << std::endl;
					myVBO.data[myVBO.count++] = (0.0f);
					myVBO.data[myVBO.count++] = (0.0f);
					myVBO.data[myVBO.count++] = (0.0f);
				}
			}
		}
		int meshVertices = mesh->mNumVertices;
		totalVertices += meshVertices;
		meshSizes.push_back((myVBO.debugCount - sizeBefore));
	}
	glActiveTexture(GL_TEXTURE0);
	numberOfMaterials = scene->mNumMaterials;
	std::vector<int> materialRemap(numberOfMaterials);
	for (int x = 0; x < numberOfMaterials; x++){
		const aiMaterial* material = scene->mMaterials[x];
		int texIndex = 0;
		aiString path;
		material->GetTexture(aiTextureType_DIFFUSE, texIndex, &path);
		std::cout << "Need Material: " << path.data <<  std::endl;
		if (material->GetTexture(aiTextureType_DIFFUSE, texIndex, &path) == AI_SUCCESS){
			std::string dir = GetDirectoryPath(filePath);
			std::string textureName = path.data;
			std::string fullPath = dir + textureName;
			std::cout << "Loaded Material: " << fullPath << std::endl;
			int texFound = -1;
			int size = textures.size();
			for (int y = 0; y < size; y++){
				if (fullPath == textures[y].GetPath()){
					texFound = y;
					break;
				}
			}
			if (texFound != -1)
				materialRemap[x] = texFound;
			else{
				Texture newTexture;
				newTexture.Load(fullPath, true);
				newTexture.SetFiltering(TEXTURE_FILTER_MAG_BILINEAR, TEXTURE_FILTER_MIN_BILINEAR_MIPMAP);
				materialRemap[x] = textures.size();
				textures.push_back(newTexture);
			}
		}
	}
	int size = meshSizes.size();
	for (int x = 0; x < size; x++){
		int oldIndex = materialIndices[x];
		materialIndices[x] = materialRemap[oldIndex];
	}
	for (int x = 0; x < textures.size(); x++){
		std::cout << "\t" << textures[x].GetPath() << std::endl;
	}
	std::cout << "Loaded Model Successfully (probobly)" << std::endl;
	isLoaded = true;
	FinalizeVBO();
	return true;
};
void Model::FinalizeVBO(){
	glGenVertexArrays(1, &VAOid);
	std::cout << "VAO: " << std::to_string(VAOid) << std::endl;
	glBindVertexArray(VAOid);
	myVBO.BindCreate();
	myVBO.Upload(GL_STATIC_DRAW);
	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, neededSize, 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, neededSize, (void*)(sizeof(float)*3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, neededSize, (void*)(sizeof(float)*5));
	glBindVertexArray(0);
};
void Model::BindModelsVAO(){
	glBindVertexArray(VAOid);
};
void Model::Render(){
	if (!isLoaded)
		return;
	BindModelsVAO();
	int size = meshSizes.size();
	for(int x = 0; x < size; x++){
		int iMatIndex = materialIndices[x];
		textures[iMatIndex].Bind();
		//std::cout <<textures[iMatIndex].GetPath() << std::endl;
		glDrawArrays(GL_TRIANGLES, meshStartIndices[x], meshSizes[x]);
	}
	glBindVertexArray(0);
};
void Model::Update(glm::mat4 model){
	shader->Use();
	shader->SetModelAndNormalMatrix("modelMatrix", "normalMatrix", model);
}