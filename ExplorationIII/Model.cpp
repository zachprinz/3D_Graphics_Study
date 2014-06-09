#include "Model.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <btBulletDynamicsCommon.h>
#include <exception>
#include <assert.h>
#include <stdio.h>
#include <string>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <GLFW/glfw3.h>

#define X_AXIS glm::vec3(1.f,0.f,0.f)
#define Y_AXIS glm::vec3(0.f,1.f,0.f)
#define Z_AXIS glm::vec3(0.f,0.f,1.f)


std::vector<Texture> Model::textures;
Shader* Model::shader;
float Model::neededSize = 0;
float Model::elapsedTime = 0;
map<std::string, int> Model::userMeshes;

void Model::SetAnimation(std::string animationName){
	if (isAnimated){
		currentAnimation = animations[animationName];
		loop = false;
	}
}
void Model::Loop(){ loop = true; Play(); };
void Model::Play(){ play = true; animationStartTime = glfwGetTime(); paused = false; }
void Model::Pause(){ paused = true; pauseTime = glfwGetTime() - animationStartTime; };
void Model::Resume(){ paused = false; animationStartTime = glfwGetTime() - pauseTime; };

std::string GetDirectoryPath(std::string sFilePath){
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
		neededSize = sizeof(float)* (8 + (2*NUM_BONES_PER_VEREX));
	}
	isLoaded = false;
	play = false;
	loop = false;
	paused = false;
	modelOffset = glm::vec3(0.0, 0.0, 0.0);
	modelScale = glm::vec3(1.0, 1.0, 1.0);
	currentAnimation = 88;
};
bool Model::Load(char* filepath){
	std::cout << "Loading Model: " << filepath << std::endl;
	startTime = glfwGetTime();
	this->filePath = filepath;
	glGenBuffers(NUM_VBs, buffers);
	glGenVertexArrays(1, &VAOid);
	glBindVertexArray(VAOid);
	scene = importer.ReadFile(filepath, aiProcess_Triangulate | aiProcess_GenSmoothNormals);
	std::cout << "\tImported Model: " << glfwGetTime() - startTime;
	if (scene){
		globalInverseTransform = scene->mRootNode->mTransformation;
		globalInverseTransform.Inverse();
		InitFromScene();
	} else {
		std::cout << "\tError Loading Model" << std::endl;
		return false;
	}
}
bool Model::InitFromScene(){
	meshes.resize(scene->mNumMeshes);
	isAnimated = scene->HasAnimations();
	if (meshes.size() > 10)
		SetUserMeshes();
	vector<glm::vec3> Positions;
	vector<glm::vec3> Normals;
	vector<glm::vec2> TexCoords;
	vector<VertexBoneData> Bones;
	vector<uint> Indices;

	uint NumVertices = 0;
	uint NumIndices = 0;

	for (uint i = 0; i < meshes.size(); i++) {
		meshes[i].MaterialIndex = scene->mMeshes[i]->mMaterialIndex;
		meshes[i].NumIndices = scene->mMeshes[i]->mNumFaces * 3;
		meshes[i].BaseVertex = NumVertices;
		meshes[i].BaseIndex = NumIndices;
		meshes[i].draw = true; // Check later
		aiString tempPath;
		int tempTexLoc = 0;
		if (scene->mMaterials[scene->mMeshes[i]->mMaterialIndex]->GetTexture(aiTextureType_DIFFUSE, tempTexLoc, &tempPath) == AI_SUCCESS)
			meshes[i].name = tempPath.C_Str();
		meshes[i].materialPath = GetDirectoryPath(filePath) + meshes[i].name;
		meshes[i].name = meshes[i].name.substr(0, meshes[i].name.find("."));
		if (scene->mNumMeshes > 100 && userMeshes.find(meshes[i].name) == userMeshes.end())
			meshes[i].draw = false;
		if (meshes[i].draw){
			NumVertices += scene->mMeshes[i]->mNumVertices;
			NumIndices += meshes[i].NumIndices;
		}
	}

	Positions.reserve(NumVertices);
	Normals.reserve(NumVertices);
	TexCoords.reserve(NumVertices);
	Bones.resize(NumVertices);
	Indices.reserve(NumIndices);

	for (uint i = 0; i < meshes.size(); i++) {
		const aiMesh* paiMesh = scene->mMeshes[i];
		if (meshes[i].draw){
			InitMesh(i, paiMesh, Positions, Normals, TexCoords, Bones, Indices);
			std::cout << "\tFinished Initiating Mesh: " << meshes[i].name << " \tTime: " << glfwGetTime() - startTime << std::endl;
		}
	}
	std::cout << "\tFinished Initiating Textures: " << glfwGetTime() - startTime << std::endl;
	LoadTextures();
	std::cout << "\tFinished Initiating Textures: " << glfwGetTime() - startTime << std::endl;

	shader->Use();

	glBindBuffer(GL_ARRAY_BUFFER, buffers[POS_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Positions[0]) * Positions.size(), &Positions[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, buffers[TEXCOORD_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TexCoords[0]) * TexCoords.size(), &TexCoords[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, buffers[NORMAL_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Normals[0]) * Normals.size(), &Normals[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, buffers[BONE_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Bones[0]) * Bones.size(), &Bones[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(3);
	glVertexAttribIPointer(3, 4, GL_INT, sizeof(VertexBoneData), (const GLvoid*)0);
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData), (const GLvoid*)16);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[INDEX_BUFFER]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices[0]) * Indices.size(), &Indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);

	return true;
}

void Model::InitMesh(uint MeshIndex, const aiMesh* paiMesh,vector<glm::vec3>& Positions,vector<glm::vec3>& Normals,vector<glm::vec2>& TexCoords,vector<VertexBoneData>& Bones,vector<uint>& Indices)
{

	const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);
	// Populate the vertex attribute vectors
	for (uint i = 0; i < paiMesh->mNumVertices; i++) {
		const aiVector3D* pPos = &(paiMesh->mVertices[i]);
		const aiVector3D* pNormal = &(paiMesh->mNormals[i]);
		const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0) ? &(paiMesh->mTextureCoords[0][i]) : &Zero3D;

		Positions.push_back(glm::vec3(pPos->x, pPos->y, pPos->z));
		Normals.push_back(glm::vec3(pNormal->x, pNormal->y, pNormal->z));
		TexCoords.push_back(glm::vec2(pTexCoord->x, pTexCoord->y));
	}
	std::cout << "\t\Loading Bones: ... ";
	LoadBones(MeshIndex, paiMesh, Bones);
	std::cout << glfwGetTime() - startTime << std::endl;

	// Populate the index buffer
	for (uint i = 0; i < paiMesh->mNumFaces; i++) {
		const aiFace& Face = paiMesh->mFaces[i];
		assert(Face.mNumIndices == 3);
		Indices.push_back(Face.mIndices[0]);
		Indices.push_back(Face.mIndices[1]);
		Indices.push_back(Face.mIndices[2]);
	}
}


bool Model::LoadTextures(){
	if (scene->mNumMeshes == 6)
		bool debug239u = true;
	numberOfMaterials = 0;
	for (int x = 0; x < scene->mNumMeshes; x++){
		if (meshes[x].draw){
			materialIndices.push_back(scene->mMeshes[x]->mMaterialIndex);
			numberOfMaterials++;
		}
	}
	glActiveTexture(GL_TEXTURE0);
	for (int x = 0; x < meshes.size(); x++){
		if (meshes[x].draw){
			std::string fullPath = meshes[x].materialPath;
			int texFound = -1;
			int size = textures.size();
			for (int y = 0; y < size; y++){
				if (fullPath == textures[y].GetPath()){
					texFound = y;
					break;
				}
			}
			if (texFound != -1){
				meshes[x].MaterialIndex = texFound;
			}
			else{
				Texture newTexture;
				newTexture.Load(fullPath, true);
				newTexture.SetFiltering(TEXTURE_FILTER_MAG_BILINEAR, TEXTURE_FILTER_MIN_BILINEAR_MIPMAP);
				meshes[x].MaterialIndex = textures.size();
				textures.push_back(newTexture);
			}
		}
	}
	isLoaded = true;
	FinalizeVBO();
	return true;
};
void Model::FinalizeVBO(){

};
void Model::BindModelsVAO(){
	glBindVertexArray(VAOid);
};
void Model::Render(){
	shader->Use();
	if (!isLoaded)
		return;
	BindModelsVAO();
	int size = meshes.size();
	for(int x = 0; x < size; x++){
		if (meshes[x].draw){
			textures[meshes[x].MaterialIndex].Bind();
			glDrawElementsBaseVertex(GL_TRIANGLES, meshes[x].NumIndices, GL_UNSIGNED_INT, (void*)(sizeof(uint)* meshes[x].BaseIndex), meshes[x].BaseVertex);
		}
	}
	glBindVertexArray(0);
};
void Model::SetRotation(glm::vec3 rot){
	rotation = rot;
}
void Model::Update(glm::mat4 model){
	shader->Use();
	glm::mat4 tempModel = model;
	tempModel = glm::scale(tempModel, modelScale);
	tempModel = glm::rotate(tempModel, rotation.x, X_AXIS);
	tempModel = glm::rotate(tempModel, rotation.y, Y_AXIS);
	tempModel = glm::rotate(tempModel, rotation.z, Z_AXIS);
	tempModel = glm::translate(tempModel, modelOffset);
	shader->SetModelAndNormalMatrix("modelMatrix", "normalMatrix", tempModel);
	vector<Matrix4f> Transforms;
	if (isAnimated){
		BoneTransform(Transforms);
		for (int x = 0; x < Transforms.size(); x++){
			char tempName[128];
			memset(tempName, 0, sizeof(tempName));
			sprintf_s(tempName, "gBones[%d]", x);
			GLuint tempPos = glGetUniformLocation(shader->ID, tempName);
			Matrix4f tempMat = Transforms[x];
			glUniformMatrix4fv(tempPos, 1, GL_TRUE, tempMat.m[0]);
		}
	}
}

void Model::LoadBones(uint MeshIndex, const aiMesh* pMesh, vector<VertexBoneData>& Bones){
	if (isAnimated){
		for (int x = 0; x < scene->mNumAnimations; x++){
			animations[scene->mAnimations[x]->mName.C_Str()] = x;
		}
	}
	for (uint i = 0; i < pMesh->mNumBones; i++) {
		uint BoneIndex = 0;
		string BoneName(pMesh->mBones[i]->mName.data);

		if (boneMap.find(BoneName) == boneMap.end()) {
			BoneIndex = boneCount;
			boneCount++;
			BoneInfo bi;
			bi.FinalTransformation = Matrix4f();
			bi.FinalTransformation.InitIdentity();
			boneInfo.push_back(bi);
		}
		else {
			BoneIndex = boneMap[BoneName];
		}
		boneMap[BoneName] = BoneIndex;
		boneInfo[BoneIndex].BoneOffset = pMesh->mBones[i]->mOffsetMatrix;
		for (uint j = 0; j < pMesh->mBones[i]->mNumWeights; j++) {
			uint VertexID = meshes[MeshIndex].BaseVertex + pMesh->mBones[i]->mWeights[j].mVertexId;
			float Weight = pMesh->mBones[i]->mWeights[j].mWeight;
			Bones[VertexID].AddBoneData(BoneIndex, Weight);
		}
	}
}
bool Model::BoneTransform(vector<Matrix4f>& Transforms){
	if (isAnimated){
		if (play == false && loop == false){//yeah I know !loop and !play ... I just didn't use them. Deal with it, fuck you.
			SetAnimation("IdleA");
			Loop();
		}
		float timeSinceAnimationStart = (glfwGetTime() - animationStartTime);
		if (paused)
			timeSinceAnimationStart = pauseTime;
		if (play){
			Matrix4f Identity;
			Identity.InitIdentity();
			float TicksPerSecond = scene->mAnimations[currentAnimation]->mTicksPerSecond != 0 ? scene->mAnimations[0]->mTicksPerSecond : 25.0f;
			float TimeInTicks = (timeSinceAnimationStart)* TicksPerSecond;
			if (TimeInTicks >= scene->mAnimations[currentAnimation]->mDuration)
				play = false;
			else{
				ReadNodeHeirarchy(TimeInTicks, scene->mRootNode, Identity);
				Transforms.resize(boneCount);
				for (uint i = 0; i < boneCount; i++) {
					Transforms[i] = boneInfo[i].FinalTransformation;
				}
				return true;
			}
		}
		if (play == false && loop){
			float endTime = scene->mAnimations[currentAnimation]->mDuration + 2;
			float TicksPerSecond = scene->mAnimations[currentAnimation]->mTicksPerSecond != 0 ? scene->mAnimations[0]->mTicksPerSecond : 25.0f;
			float TimeInTicks = (timeSinceAnimationStart)* TicksPerSecond;
			if (TimeInTicks >= endTime)
				Play();
			else{
				std::cout << "Resolving: " << TimeInTicks << std::endl;
				Matrix4f Identity;
				Identity.InitIdentity();
				ReadNodeHeirarchy(TimeInTicks, scene->mRootNode, Identity, true);
				Transforms.resize(boneCount);
				for (uint i = 0; i < boneCount; i++) {
					Transforms[i] = boneInfo[i].FinalTransformation;
				}
				return true;
			}
			return true;
		}
		return false;
	}
	return false;
}
void Model::ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const Matrix4f& ParentTransform, bool resolve)
{
	string NodeName(pNode->mName.data);

	const aiAnimation* pAnimation = scene->mAnimations[currentAnimation];

	Matrix4f NodeTransformation(pNode->mTransformation);

	const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, NodeName);

	if (pNodeAnim) {
		// Interpolate scaling and generate scaling transformation matrix
		aiVector3D Scaling;
		CalcInterpolatedScaling(Scaling, AnimationTime, pNodeAnim, resolve);
		Matrix4f ScalingM;
		ScalingM.InitScaleTransform(Scaling.x, Scaling.y, Scaling.z);

		// Interpolate rotation and generate rotation transformation matrix
		aiQuaternion RotationQ;
		CalcInterpolatedRotation(RotationQ, AnimationTime, pNodeAnim, resolve);
		Matrix4f RotationM = Matrix4f(RotationQ.GetMatrix());

		// Interpolate translation and generate translation transformation matrix
		aiVector3D Translation;
		CalcInterpolatedPosition(Translation, AnimationTime, pNodeAnim, resolve);
		Matrix4f TranslationM;
		TranslationM.InitTranslationTransform(Translation.x, Translation.y, Translation.z);

		// Combine the above transformations
		NodeTransformation = TranslationM * RotationM * ScalingM;
	}

	Matrix4f GlobalTransformation = ParentTransform * NodeTransformation;

	if (boneMap.find(NodeName) != boneMap.end()) {
		uint BoneIndex = boneMap[NodeName];
		boneInfo[BoneIndex].FinalTransformation = globalInverseTransform * GlobalTransformation * boneInfo[BoneIndex].BoneOffset;
	}

	for (uint i = 0; i < pNode->mNumChildren; i++) {
		ReadNodeHeirarchy(AnimationTime, pNode->mChildren[i], GlobalTransformation, resolve);
	}
}
void Model::SetUserMeshes(){
	//Base Body
	userMeshes["OBM_Arms"] = 1;
	userMeshes["HairMeshA"] = 1;
	userMeshes["OBM_Head_EarsA"] = 1;
	userMeshes["OBM_Legs"] = 1;
	userMeshes["OBM_Torso"] = 1;
	userMeshes["noseB"] = 1;
	userMeshes["OBM_Head_LowerTeeth"] = 1;
	userMeshes["OBM_Head_UpperTeeth"] = 1;
	userMeshes["OBheadStyleB"] = 1;
	userMeshes["BOOTSleatherArmour"] = 1;
}
void Model::SetModelOffset(glm::vec3 newOffset){
	modelOffset = newOffset;
}
void Model::SetModelScale(glm::vec3 newScale){
	modelScale = newScale;
}
glm::vec3 Model::GetModelOffset(){
	return modelOffset;
}
uint Model::FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim){
	for (uint i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime) {
			return i;
		}
	}

	assert(0);

	return 0;
}
uint Model::FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim){
	assert(pNodeAnim->mNumRotationKeys > 0);

	for (uint i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime) {
			return i;
		}
	}

	assert(0);

	return 0;
}
uint Model::FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim){
	assert(pNodeAnim->mNumScalingKeys > 0);

	for (uint i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime) {
			return i;
		}
	}

	assert(0);

	return 0;
}
void Model::CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim, bool resolve){
	if (pNodeAnim->mNumPositionKeys == 1) {
		Out = pNodeAnim->mPositionKeys[0].mValue;
		return;
	}
	if (!resolve){
		uint PositionIndex = FindPosition(AnimationTime, pNodeAnim);
		uint NextPositionIndex = (PositionIndex + 1);
		assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
		float DeltaTime = (float)(pNodeAnim->mPositionKeys[NextPositionIndex].mTime - pNodeAnim->mPositionKeys[PositionIndex].mTime);
		float Factor = (AnimationTime - (float)pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
		//assert(Factor >= 0.0f && Factor <= 1.0f);
		const aiVector3D& Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
		const aiVector3D& End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
		aiVector3D Delta = End - Start;
		Out = Start + Factor * Delta;
	}
	else{
		uint PositionIndex = pNodeAnim->mNumPositionKeys - 1;
		uint NextPositionIndex = 1;
		float DeltaTime = (float)((pNodeAnim->mPositionKeys[PositionIndex].mTime + 2) - pNodeAnim->mPositionKeys[PositionIndex].mTime);
		float Factor = (AnimationTime - ((float)pNodeAnim->mPositionKeys[PositionIndex].mTime)) / DeltaTime;
		assert(Factor >= 0.0f && Factor <= 1.0f);
		const aiVector3D& Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
		const aiVector3D& End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
		aiVector3D Delta = End - Start;
		Out = Start + Factor * Delta;
	}
}
void Model::CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim, bool resolve){
	// we need at least two values to interpolate...
	if (pNodeAnim->mNumRotationKeys == 1) {
		Out = pNodeAnim->mRotationKeys[0].mValue;
		return;
	}
	if (!resolve){
		uint RotationIndex = FindRotation(AnimationTime, pNodeAnim);
		uint NextRotationIndex = (RotationIndex + 1);
		assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
		float DeltaTime = (float)(pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime);
		float Factor = (AnimationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
		//assert(Factor >= 0.0f && Factor <= 1.0f);
		const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
		const aiQuaternion& EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
		aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
		Out = Out.Normalize();
	} else {
		uint RotationIndex = pNodeAnim->mNumScalingKeys - 1;
		uint NextRotationIndex = 1;
		float DeltaTime = (float)((pNodeAnim->mRotationKeys[RotationIndex].mTime + 2) - pNodeAnim->mRotationKeys[RotationIndex].mTime);
		float Factor = (AnimationTime - ((float)pNodeAnim->mRotationKeys[RotationIndex].mTime)) / DeltaTime;
		assert(Factor >= 0.0f && Factor <= 1.0f);
		const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
		const aiQuaternion& EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
		aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
		Out = Out.Normalize();
	}
}
void Model::CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim, bool resolve){
	if (pNodeAnim->mNumScalingKeys == 1) {
		Out = pNodeAnim->mScalingKeys[0].mValue;
		return;
	}
	if (!resolve){
		uint ScalingIndex = FindScaling(AnimationTime, pNodeAnim);
		uint NextScalingIndex = (ScalingIndex + 1);
		assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
		float DeltaTime = (float)(pNodeAnim->mScalingKeys[NextScalingIndex].mTime - pNodeAnim->mScalingKeys[ScalingIndex].mTime);
		float Factor = (AnimationTime - (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;
		//assert(Factor >= 0.0f && Factor <= 1.0f); //Not working
		const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
		const aiVector3D& End = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
		aiVector3D Delta = End - Start;
		Out = Start + Factor * Delta;
	}
	else{
		uint ScalingIndex = pNodeAnim->mNumScalingKeys - 1;
		uint NextScalingIndex = 1;
		float DeltaTime = (float)((pNodeAnim->mScalingKeys[ScalingIndex].mTime + 2) - pNodeAnim->mScalingKeys[ScalingIndex].mTime);
		float Factor = (AnimationTime - ((float)pNodeAnim->mScalingKeys[ScalingIndex].mTime)) / DeltaTime;
		assert(Factor >= 0.0f && Factor <= 1.0f); //Not working
		const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
		const aiVector3D& End = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
		aiVector3D Delta = End - Start;
		Out = Start + Factor * Delta;
	}
}
const aiNodeAnim* Model::FindNodeAnim(const aiAnimation* pAnimation, const string NodeName){
	for (uint i = 0; i < pAnimation->mNumChannels; i++) {
		const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];

		if (string(pNodeAnim->mNodeName.data) == NodeName) {
			return pNodeAnim;
		}
	}

	return NULL;
}

void Model::VertexBoneData::AddBoneData(uint BoneID, float Weight) {
	for (uint i = 0; i < NUM_BONES_PER_VEREX; i++) {
		if (Weights[i] == 0.0) {
			IDs[i] = BoneID;
			Weights[i] = Weight;
			return;
		}
	}
	// should never get here - more bones than we have space for
	//assert(0);
}
void Matrix4f::InitScaleTransform(float ScaleX, float ScaleY, float ScaleZ)
{
	m[0][0] = ScaleX; m[0][1] = 0.0f;   m[0][2] = 0.0f;   m[0][3] = 0.0f;
	m[1][0] = 0.0f;   m[1][1] = ScaleY; m[1][2] = 0.0f;   m[1][3] = 0.0f;
	m[2][0] = 0.0f;   m[2][1] = 0.0f;   m[2][2] = ScaleZ; m[2][3] = 0.0f;
	m[3][0] = 0.0f;   m[3][1] = 0.0f;   m[3][2] = 0.0f;   m[3][3] = 1.0f;
}

void Matrix4f::InitRotateTransform(float RotateX, float RotateY, float RotateZ)
{
	Matrix4f rx, ry, rz;

	const float x = ToRadian(RotateX);
	const float y = ToRadian(RotateY);
	const float z = ToRadian(RotateZ);

	rx.m[0][0] = 1.0f; rx.m[0][1] = 0.0f; rx.m[0][2] = 0.0f; rx.m[0][3] = 0.0f;
	rx.m[1][0] = 0.0f; rx.m[1][1] = cosf(x); rx.m[1][2] = -sinf(x); rx.m[1][3] = 0.0f;
	rx.m[2][0] = 0.0f; rx.m[2][1] = sinf(x); rx.m[2][2] = cosf(x); rx.m[2][3] = 0.0f;
	rx.m[3][0] = 0.0f; rx.m[3][1] = 0.0f; rx.m[3][2] = 0.0f; rx.m[3][3] = 1.0f;

	ry.m[0][0] = cosf(y); ry.m[0][1] = 0.0f; ry.m[0][2] = -sinf(y); ry.m[0][3] = 0.0f;
	ry.m[1][0] = 0.0f; ry.m[1][1] = 1.0f; ry.m[1][2] = 0.0f; ry.m[1][3] = 0.0f;
	ry.m[2][0] = sinf(y); ry.m[2][1] = 0.0f; ry.m[2][2] = cosf(y); ry.m[2][3] = 0.0f;
	ry.m[3][0] = 0.0f; ry.m[3][1] = 0.0f; ry.m[3][2] = 0.0f; ry.m[3][3] = 1.0f;

	rz.m[0][0] = cosf(z); rz.m[0][1] = -sinf(z); rz.m[0][2] = 0.0f; rz.m[0][3] = 0.0f;
	rz.m[1][0] = sinf(z); rz.m[1][1] = cosf(z); rz.m[1][2] = 0.0f; rz.m[1][3] = 0.0f;
	rz.m[2][0] = 0.0f; rz.m[2][1] = 0.0f; rz.m[2][2] = 1.0f; rz.m[2][3] = 0.0f;
	rz.m[3][0] = 0.0f; rz.m[3][1] = 0.0f; rz.m[3][2] = 0.0f; rz.m[3][3] = 1.0f;

	*this = rz * ry * rx;
}

void Matrix4f::InitTranslationTransform(float x, float y, float z)
{
	m[0][0] = 1.0f; m[0][1] = 0.0f; m[0][2] = 0.0f; m[0][3] = x;
	m[1][0] = 0.0f; m[1][1] = 1.0f; m[1][2] = 0.0f; m[1][3] = y;
	m[2][0] = 0.0f; m[2][1] = 0.0f; m[2][2] = 1.0f; m[2][3] = z;
	m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 1.0f;
}

float Matrix4f::Determinant() const
{
	return m[0][0] * m[1][1] * m[2][2] * m[3][3] - m[0][0] * m[1][1] * m[2][3] * m[3][2] + m[0][0] * m[1][2] * m[2][3] * m[3][1] - m[0][0] * m[1][2] * m[2][1] * m[3][3]
		+ m[0][0] * m[1][3] * m[2][1] * m[3][2] - m[0][0] * m[1][3] * m[2][2] * m[3][1] - m[0][1] * m[1][2] * m[2][3] * m[3][0] + m[0][1] * m[1][2] * m[2][0] * m[3][3]
		- m[0][1] * m[1][3] * m[2][0] * m[3][2] + m[0][1] * m[1][3] * m[2][2] * m[3][0] - m[0][1] * m[1][0] * m[2][2] * m[3][3] + m[0][1] * m[1][0] * m[2][3] * m[3][2]
		+ m[0][2] * m[1][3] * m[2][0] * m[3][1] - m[0][2] * m[1][3] * m[2][1] * m[3][0] + m[0][2] * m[1][0] * m[2][1] * m[3][3] - m[0][2] * m[1][0] * m[2][3] * m[3][1]
		+ m[0][2] * m[1][1] * m[2][3] * m[3][0] - m[0][2] * m[1][1] * m[2][0] * m[3][3] - m[0][3] * m[1][0] * m[2][1] * m[3][2] + m[0][3] * m[1][0] * m[2][2] * m[3][1]
		- m[0][3] * m[1][1] * m[2][2] * m[3][0] + m[0][3] * m[1][1] * m[2][0] * m[3][2] - m[0][3] * m[1][2] * m[2][0] * m[3][1] + m[0][3] * m[1][2] * m[2][1] * m[3][0];
}


Matrix4f& Matrix4f::Inverse()
{
	float det = Determinant();
	if (det == 0.0f)
	{
		assert(0);
		return *this;
	}

	float invdet = 1.0f / det;

	Matrix4f res;
	res.m[0][0] = invdet  * (m[1][1] * (m[2][2] * m[3][3] - m[2][3] * m[3][2]) + m[1][2] * (m[2][3] * m[3][1] - m[2][1] * m[3][3]) + m[1][3] * (m[2][1] * m[3][2] - m[2][2] * m[3][1]));
	res.m[0][1] = -invdet * (m[0][1] * (m[2][2] * m[3][3] - m[2][3] * m[3][2]) + m[0][2] * (m[2][3] * m[3][1] - m[2][1] * m[3][3]) + m[0][3] * (m[2][1] * m[3][2] - m[2][2] * m[3][1]));
	res.m[0][2] = invdet  * (m[0][1] * (m[1][2] * m[3][3] - m[1][3] * m[3][2]) + m[0][2] * (m[1][3] * m[3][1] - m[1][1] * m[3][3]) + m[0][3] * (m[1][1] * m[3][2] - m[1][2] * m[3][1]));
	res.m[0][3] = -invdet * (m[0][1] * (m[1][2] * m[2][3] - m[1][3] * m[2][2]) + m[0][2] * (m[1][3] * m[2][1] - m[1][1] * m[2][3]) + m[0][3] * (m[1][1] * m[2][2] - m[1][2] * m[2][1]));
	res.m[1][0] = -invdet * (m[1][0] * (m[2][2] * m[3][3] - m[2][3] * m[3][2]) + m[1][2] * (m[2][3] * m[3][0] - m[2][0] * m[3][3]) + m[1][3] * (m[2][0] * m[3][2] - m[2][2] * m[3][0]));
	res.m[1][1] = invdet  * (m[0][0] * (m[2][2] * m[3][3] - m[2][3] * m[3][2]) + m[0][2] * (m[2][3] * m[3][0] - m[2][0] * m[3][3]) + m[0][3] * (m[2][0] * m[3][2] - m[2][2] * m[3][0]));
	res.m[1][2] = -invdet * (m[0][0] * (m[1][2] * m[3][3] - m[1][3] * m[3][2]) + m[0][2] * (m[1][3] * m[3][0] - m[1][0] * m[3][3]) + m[0][3] * (m[1][0] * m[3][2] - m[1][2] * m[3][0]));
	res.m[1][3] = invdet  * (m[0][0] * (m[1][2] * m[2][3] - m[1][3] * m[2][2]) + m[0][2] * (m[1][3] * m[2][0] - m[1][0] * m[2][3]) + m[0][3] * (m[1][0] * m[2][2] - m[1][2] * m[2][0]));
	res.m[2][0] = invdet  * (m[1][0] * (m[2][1] * m[3][3] - m[2][3] * m[3][1]) + m[1][1] * (m[2][3] * m[3][0] - m[2][0] * m[3][3]) + m[1][3] * (m[2][0] * m[3][1] - m[2][1] * m[3][0]));
	res.m[2][1] = -invdet * (m[0][0] * (m[2][1] * m[3][3] - m[2][3] * m[3][1]) + m[0][1] * (m[2][3] * m[3][0] - m[2][0] * m[3][3]) + m[0][3] * (m[2][0] * m[3][1] - m[2][1] * m[3][0]));
	res.m[2][2] = invdet  * (m[0][0] * (m[1][1] * m[3][3] - m[1][3] * m[3][1]) + m[0][1] * (m[1][3] * m[3][0] - m[1][0] * m[3][3]) + m[0][3] * (m[1][0] * m[3][1] - m[1][1] * m[3][0]));
	res.m[2][3] = -invdet * (m[0][0] * (m[1][1] * m[2][3] - m[1][3] * m[2][1]) + m[0][1] * (m[1][3] * m[2][0] - m[1][0] * m[2][3]) + m[0][3] * (m[1][0] * m[2][1] - m[1][1] * m[2][0]));
	res.m[3][0] = -invdet * (m[1][0] * (m[2][1] * m[3][2] - m[2][2] * m[3][1]) + m[1][1] * (m[2][2] * m[3][0] - m[2][0] * m[3][2]) + m[1][2] * (m[2][0] * m[3][1] - m[2][1] * m[3][0]));
	res.m[3][1] = invdet  * (m[0][0] * (m[2][1] * m[3][2] - m[2][2] * m[3][1]) + m[0][1] * (m[2][2] * m[3][0] - m[2][0] * m[3][2]) + m[0][2] * (m[2][0] * m[3][1] - m[2][1] * m[3][0]));
	res.m[3][2] = -invdet * (m[0][0] * (m[1][1] * m[3][2] - m[1][2] * m[3][1]) + m[0][1] * (m[1][2] * m[3][0] - m[1][0] * m[3][2]) + m[0][2] * (m[1][0] * m[3][1] - m[1][1] * m[3][0]));
	res.m[3][3] = invdet  * (m[0][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1]) + m[0][1] * (m[1][2] * m[2][0] - m[1][0] * m[2][2]) + m[0][2] * (m[1][0] * m[2][1] - m[1][1] * m[2][0]));
	*this = res;

	return *this;
}

Quaternion::Quaternion(float _x, float _y, float _z, float _w)
{
	x = _x;
	y = _y;
	z = _z;
	w = _w;
}

void Quaternion::Normalize()
{
	float Length = sqrtf(x * x + y * y + z * z + w * w);

	x /= Length;
	y /= Length;
	z /= Length;
	w /= Length;
}


Quaternion Quaternion::Conjugate()
{
	Quaternion ret(-x, -y, -z, w);
	return ret;
}

Quaternion operator*(const Quaternion& l, const Quaternion& r)
{
	const float w = (l.w * r.w) - (l.x * r.x) - (l.y * r.y) - (l.z * r.z);
	const float x = (l.x * r.w) + (l.w * r.x) + (l.y * r.z) - (l.z * r.y);
	const float y = (l.y * r.w) + (l.w * r.y) + (l.z * r.x) - (l.x * r.z);
	const float z = (l.z * r.w) + (l.w * r.z) + (l.x * r.y) - (l.y * r.x);

	Quaternion ret(x, y, z, w);

	return ret;
}