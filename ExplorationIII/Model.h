#ifndef MODEL_H
#define MODEL_H

#include "VBO.h"
#include "Material.h"
#include "Shader.h"
#include "Texture.h"
#include <algorithm>
#include <cmath>
#include <cstring>
#include <vector>
#include <sstream>
#include <queue>
#include <map>
#include <set>
#include <vector>
#include <iostream>
#include <gl/glew.h>
#include <glm/glm.hpp>
#include "Shader.h"
#include <map>
#include <string>
#include <GL/glew.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <FI/FreeImage.h>

#define uint GLuint
#define Matrix4f aiMatrix4x4
#define NUM_BONES_PER_VEREX 4


class Model{
public:
	Model();
	bool Load(char* filePath);
	void FinalizeVBO();
	void BindModelsVAO();
	static float neededSize;
	void Update(glm::mat4);
	void Render();
	static Shader* shader;
	void BoneTransform(float TimeInSeconds, vector<Matrix4f>& Transforms);
	uint getBoneCount() const { return boneCount;  }
private:
	bool isLoaded;
	VBO myVBO;
	GLuint VAOid;
	static std::vector<Texture> textures;
	std::vector<int> meshStartIndices;
	std::vector<int> meshStartVerticies;
	std::vector<int> meshSizes;
	std::vector<int> materialIndices;
	std::vector<bool> meshIsTextured;
	int numberOfMaterials;

	//Animation

	struct BoneInfo {
		Matrix4f BoneOffset;
		Matrix4f FinalTransformation;
		BoneInfo() {
			BoneOffset = aiMatrix4x4();
			FinalTransformation = aiMatrix4x4();
		}
	};

	struct VertexBoneData{
		uint IDs[NUM_BONES_PER_VEREX];
		float Weights[NUM_BONES_PER_VEREX];
		VertexBoneData(){
			Reset();
		};
		void Reset() {
			for (int x = 0; x < NUM_BONES_PER_VEREX; x++){
				IDs[x] = 0;
				Weights[x] = 0;
			}
		}
		void AddBoneData(uint BoneID, float Weight);
	};

	Assimp::Importer importer;
	const aiScene* scene;
	map<std::string, uint> boneMap;
	uint boneCount;
	std::vector<BoneInfo> boneInfo;
	Matrix4f globalInverseTransform;
	void LoadBones(uint MeshIndex, const aiMesh* paiMesh, vector<VertexBoneData>& Bones);
	void CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
	void CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
	void CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
	uint FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);
	uint FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
	uint FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);
	const aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation, const string NodeName);
	void ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const Matrix4f& ParentTransform);
	bool InitFromScene(const aiScene* pScene, const string& Filename);
	const aiVector3D* normal;
};

#endif