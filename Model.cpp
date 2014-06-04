#include "Model.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

VBO Model::myVBO;
GLuint Model::VAOid;
std::vector<Texture> Model::textures;

std::string GetDirectoryPath(std::string sFilePath)
{
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
	isLoaded = false;
};
bool Model::Load(char* filePath){
	if (myVBO.GetID() == 0){
		myVBO.Create();
		textures.reserve(50);
	}
	Assimp::Importer importer;
	std::cout << filePath << std::endl;
	const aiScene* scene = importer.ReadFile(filePath, aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);
	if (!scene){
		std::cout << "Error Loading Model" << std::endl;
		return false;
	}
	else{
		std::cout << "Loaded Model" << std::endl;
	}
	const int vertexTotalSize = sizeof(aiVector3D)* 2 + sizeof(aiVector2D);
	int totalVertices = 0;
	for (int x = 0; x < scene->mNumMeshes; x++){
		std::cout << "Working with Mesh #: " << std::to_string(x) << std::endl;
		aiMesh* mesh = scene->mMeshes[x];
		int faceCount = mesh->mNumFaces;
		materialIndices.push_back(mesh->mMaterialIndex);
		int sizeBefore = myVBO.GetSize();
		meshStartIndices.push_back(sizeBefore / vertexTotalSize);
		for (int y = 0; y < faceCount; y++){
			const aiFace& face = mesh->mFaces[y];
			for (int z = 0; z < 3; z++){
				aiVector3D pos = mesh->mVertices[face.mIndices[z]];
				aiVector3D uv = mesh->mTextureCoords[0][face.mIndices[z]];
				aiVector3D normal = mesh->HasNormals() ? mesh->mNormals[face.mIndices[z]] : aiVector3D(1.0f, 1.0f, 1.0f);
				myVBO.AddData(&pos, sizeof(aiVector3D));
				myVBO.AddData(&uv, sizeof(aiVector2D));
				myVBO.AddData(&normal, sizeof(aiVector3D));
			}
		}
		int meshVertices = mesh->mNumVertices;
		totalVertices += meshVertices;
		meshSizes.push_back((myVBO.GetSize() - sizeBefore) / vertexTotalSize);
	}
	numberOfMaterials = scene->mNumMaterials;
	std::vector<int> materialRemap(numberOfMaterials);
	for (int x = 0; x < numberOfMaterials; x++){
		const aiMaterial* material = scene->mMaterials[x];
		int a = 5;
		int texIndex = 0;
		aiString path;
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
	std::cout << "Loaded Model Successfully (probobly)" << std::endl;
	isLoaded = true;
	return true;
};
void Model::FinalizeVBO(){
	glGenVertexArrays(1, &VAOid);
	std::cout << "VAO: " << std::to_string(VAOid) << std::endl;
	glBindVertexArray(VAOid);
	myVBO.Bind();
	myVBO.Upload(GL_STATIC_DRAW);
	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(aiVector3D)+sizeof(aiVector2D), 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(aiVector3D)+sizeof(aiVector2D), (void*)sizeof(aiVector3D));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(aiVector3D)+sizeof(aiVector2D), (void*)(sizeof(aiVector3D)+sizeof(aiVector2D)));
	glBindVertexArray(0);
};
void Model::BindModelsVAO(){
	glBindVertexArray(VAOid);
};
void Model::Render(GLuint uId){
	if (!isLoaded)
		return;
	int size = meshSizes.size();
	for(int x = 0; x < size; x++){
		int iMatIndex = materialIndices[x];
		//std::cout << textures[iMatIndex].GetPath() << std::endl;
		textures[iMatIndex].Bind();
		//glUniform1i(uId, GL_TEXTURE0);
		glDrawArrays(GL_TRIANGLES, meshStartIndices[x], meshSizes[x]);
	}
};