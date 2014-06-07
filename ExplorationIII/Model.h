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
#include <assimp/material.h>
#include <FI/FreeImage.h>
#include <iostream>
#include <fstream>
#include <assimp/vector3.h>
#include <assimp/matrix3x3.h>
#include <assimp/matrix4x4.h>
#include <cmath>
#include <math.h>

#define M_PI 3.141592654
#define ToRadian(x) (float)(((x) * M_PI / 180.0f))
#define ToDegree(x) (float)(((x) * 180.0f / M_PI))

#define uint GLuint
#define NUM_BONES_PER_VEREX 4

enum VB_TYPES {
	INDEX_BUFFER,
	POS_VB,
	NORMAL_VB,
	TEXCOORD_VB,
	BONE_VB,
	NUM_VBs
};

class Matrix4f
{
public:
	float m[4][4];
	Matrix4f(){}

	// constructor from Assimp matrix
	Matrix4f(const aiMatrix4x4& AssimpMatrix)
	{
		m[0][0] = AssimpMatrix.a1; m[0][1] = AssimpMatrix.a2; m[0][2] = AssimpMatrix.a3; m[0][3] = AssimpMatrix.a4;
		m[1][0] = AssimpMatrix.b1; m[1][1] = AssimpMatrix.b2; m[1][2] = AssimpMatrix.b3; m[1][3] = AssimpMatrix.b4;
		m[2][0] = AssimpMatrix.c1; m[2][1] = AssimpMatrix.c2; m[2][2] = AssimpMatrix.c3; m[2][3] = AssimpMatrix.c4;
		m[3][0] = AssimpMatrix.d1; m[3][1] = AssimpMatrix.d2; m[3][2] = AssimpMatrix.d3; m[3][3] = AssimpMatrix.d4;
	}

	Matrix4f(const aiMatrix3x3& AssimpMatrix)
	{
		m[0][0] = AssimpMatrix.a1; m[0][1] = AssimpMatrix.a2; m[0][2] = AssimpMatrix.a3; m[0][3] = 0.0f;
		m[1][0] = AssimpMatrix.b1; m[1][1] = AssimpMatrix.b2; m[1][2] = AssimpMatrix.b3; m[1][3] = 0.0f;
		m[2][0] = AssimpMatrix.c1; m[2][1] = AssimpMatrix.c2; m[2][2] = AssimpMatrix.c3; m[2][3] = 0.0f;
		m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 1.0f;
	}

	Matrix4f(float a00, float a01, float a02, float a03,
		float a10, float a11, float a12, float a13,
		float a20, float a21, float a22, float a23,
		float a30, float a31, float a32, float a33)
	{
		m[0][0] = a00; m[0][1] = a01; m[0][2] = a02; m[0][3] = a03;
		m[1][0] = a10; m[1][1] = a11; m[1][2] = a12; m[1][3] = a13;
		m[2][0] = a20; m[2][1] = a21; m[2][2] = a22; m[2][3] = a23;
		m[3][0] = a30; m[3][1] = a31; m[3][2] = a32; m[3][3] = a33;
	}

	void SetZero()
	{
		memset(m, 0, sizeof(m));
	}

	Matrix4f Transpose() const
	{
		Matrix4f n;

		for (unsigned int i = 0; i < 4; i++) {
			for (unsigned int j = 0; j < 4; j++) {
				n.m[i][j] = m[j][i];
			}
		}

		return n;
	}


	inline void InitIdentity()
	{
		m[0][0] = 1.0f; m[0][1] = 0.0f; m[0][2] = 0.0f; m[0][3] = 0.0f;
		m[1][0] = 0.0f; m[1][1] = 1.0f; m[1][2] = 0.0f; m[1][3] = 0.0f;
		m[2][0] = 0.0f; m[2][1] = 0.0f; m[2][2] = 1.0f; m[2][3] = 0.0f;
		m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 1.0f;
	}

	inline Matrix4f operator*(const Matrix4f& Right) const
	{
		Matrix4f Ret;

		for (unsigned int i = 0; i < 4; i++) {
			for (unsigned int j = 0; j < 4; j++) {
				Ret.m[i][j] = m[i][0] * Right.m[0][j] +
					m[i][1] * Right.m[1][j] +
					m[i][2] * Right.m[2][j] +
					m[i][3] * Right.m[3][j];
			}
		}

		return Ret;
	}
	void Print() const
	{
		for (int i = 0; i < 4; i++) {
			printf("%f %f %f %f\n", m[i][0], m[i][1], m[i][2], m[i][3]);
		}
	}

	float Determinant() const;

	Matrix4f& Inverse();

	void InitScaleTransform(float ScaleX, float ScaleY, float ScaleZ);
	void InitRotateTransform(float RotateX, float RotateY, float RotateZ);
	void InitTranslationTransform(float x, float y, float z);
};


struct Quaternion
{
	float x, y, z, w;

	Quaternion(float _x, float _y, float _z, float _w);

	void Normalize();

	Quaternion Conjugate();
};

Quaternion operator*(const Quaternion& l, const Quaternion& r);

class Model{
public:
	Model();
	bool Load(char* filePath);
	bool LoadTextures();
	void FinalizeVBO();
	void BindModelsVAO();
	static float neededSize;
	void Update(glm::mat4);
	void Render();
	static Shader* shader;
	bool BoneTransform(float TimeInSeconds, vector<Matrix4f>& Transforms);
	uint getBoneCount() const { return boneCount; }
	static float elapsedTime;
	bool isAnimated;
	map<std::string, int> animations;
	int currentAnimation;
	void SetAnimation(std::string);
private:
	void SetUserMeshes();
	static map<std::string, int> userMeshes;
	std::string filePath;
	bool InitFromScene();
	GLuint buffers[NUM_VBs];
	bool done;
	bool isLoaded;
	VBO myVBO;
	GLuint VAOid;
	static std::vector<Texture> textures;
	std::vector<int> meshStartIndices;
	std::vector<int> meshStartVerticies;
	std::vector<int> meshSizes;
	std::vector<int> materialIndices;
	std::vector<bool> meshIsTextured;
	std::vector<int> meshStartVerts;
	int numberOfMaterials;
	float startTime;
	bool loop;
	bool playing;
	float animationStartTime;
	//Animation

	struct BoneInfo {
		Matrix4f BoneOffset;
		Matrix4f FinalTransformation;
		BoneInfo() {
			memset(&BoneOffset, 0, sizeof(BoneOffset));
			memset(&FinalTransformation, 0, sizeof(FinalTransformation));
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
	struct Mesh {
		Mesh()
		{
			NumIndices = 0;
			BaseVertex = 0;
			BaseIndex = 0;
			MaterialIndex = 0;
		}
		std::string materialPath;
		std::string name;
		bool draw;
		unsigned int NumIndices;
		unsigned int BaseVertex;
		unsigned int BaseIndex;
		unsigned int MaterialIndex;
	};
	void InitMesh(uint MeshIndex,
		const aiMesh* paiMesh,
		vector<glm::vec3>& Positions,
		vector<glm::vec3>& Normals,
		vector<glm::vec2>& TexCoords,
		vector<VertexBoneData>& Bones,
		vector<unsigned int>& Indices);
	std::vector<Mesh> meshes;
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
	GLuint boneLocations;
	ofstream myfile2;
};
#endif

