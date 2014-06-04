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
#include <iomanip>      // std::setprecision


std::vector<Texture> Model::textures;
Shader* Model::shader;
float Model::neededSize = 0;
float Model::elapsedTime = 0;

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
		neededSize = sizeof(float)* (8 + (2*NUM_BONES_PER_VEREX));
		std::cout << "Needed Size: " << std::to_string(neededSize) << std::endl;
	}
	isLoaded = false;
};
bool Model::Load(char* filePath){
	scene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType | AI_SCENE_FLAGS_INCOMPLETE);// aiProcess_FlipUV's
	//scene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);
	int vertexCount = 0;
	for (int x = 0; x < scene->mNumMeshes; x++){
		vertexCount += 3*scene->mMeshes[x]->mNumFaces;
	}
	int tempTotal = 0;
	for (int x = 0; x < scene->mNumMeshes; x++){
		int sizeBefore = tempTotal;
		meshStartIndices.push_back(sizeBefore);
		for (int y = 0; y < scene->mMeshes[x]->mNumFaces; y++){
			for (int z = 0; z < 3; z++){
				tempTotal++;
			}
		}
		meshSizes.push_back((tempTotal - sizeBefore));
	}
	myVBO.Create((tempTotal * (8 + NUM_BONES_PER_VEREX) * sizeof(float)) + (tempTotal * NUM_BONES_PER_VEREX * sizeof(int)));
	if (scene){
		globalInverseTransform = scene->mRootNode->mTransformation;
		globalInverseTransform.Inverse();

	} else {
		std::cout << "Error Loading Model" << std::endl;
		return false;
	}

	vector<VertexBoneData> Bones;

	Bones.resize(tempTotal + 1);
	for (int x = 0; x < scene->mNumMeshes; x++){
		LoadBones(x, scene->mMeshes[x], Bones);
	}
	int totalVertices = 0;
	for (int x = 0; x < scene->mNumMeshes; x++){
		aiMesh* mesh = scene->mMeshes[x];
		int faceCount = mesh->mNumFaces;
		materialIndices.push_back(mesh->mMaterialIndex);
		const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);
		for (int y = 0; y < faceCount; y++){
			const aiFace& face = mesh->mFaces[y];
			for (int z = 0; z < 3; z++){
				const aiVector3D* pos = &(mesh->mVertices[face.mIndices[z]]);
				const aiVector3D* normal = &(mesh->mNormals[face.mIndices[z]]);
				const aiVector3D* uv = &(mesh->mTextureCoords[0][face.mIndices[z]]);
				std::vector<int> boneIDs;
				std::vector<float> myWeights;
				for (int currentBoneID = 0; currentBoneID < NUM_BONES_PER_VEREX; currentBoneID++){
					boneIDs.push_back(Bones.at(myVBO.debugCount).IDs[currentBoneID]);
					myWeights.push_back(Bones.at(myVBO.debugCount).Weights[currentBoneID]);
				}
				if (pos != NULL){
					myVBO.data[myVBO.count++] = (pos->x);
					myVBO.data[myVBO.count++] = (pos->y);
					myVBO.data[myVBO.count++] = (pos->z);
				}
				else{
					myVBO.data[myVBO.count++] = (0.0f);
					myVBO.data[myVBO.count++] = (0.0f);
					myVBO.data[myVBO.count++] = (0.0f);
				}
				if (uv != NULL && mesh->HasTextureCoords(0)){
					myVBO.data[myVBO.count++] = (uv->x);
					myVBO.data[myVBO.count++] = (uv->y);
				}
				else{
					myVBO.data[myVBO.count++] = (0.0f);
					myVBO.data[myVBO.count++] = (0.0f);
				}
				if (mesh->HasNormals() && normal != NULL){
					myVBO.data[myVBO.count++] = normal->x;
					myVBO.data[myVBO.count++] = normal->y;
					myVBO.data[myVBO.count++] = normal->z;
				}
				else{
					myVBO.data[myVBO.count++] = (0.0f);
					myVBO.data[myVBO.count++] = (0.0f);
					myVBO.data[myVBO.count++] = (0.0f);
				}
				for (int myBoneData = 0; myBoneData < NUM_BONES_PER_VEREX; myBoneData++){
					myVBO.data[myVBO.count++] = (float)boneIDs.at(myBoneData);
				}
				for (int myWeightData = 0; myWeightData < NUM_BONES_PER_VEREX; myWeightData++){
					myVBO.data[myVBO.count++] = myWeights.at(myWeightData);
				}
				myVBO.debugCount++;
			}
		}
		int meshVertices = mesh->mNumVertices;
		totalVertices += meshVertices;
	}
	if (false && scene->mNumMeshes == 18){
		int debugCount = 0;
		for (int x = 0; x < myVBO.debugCount; x++){
			debugCount += 8;
			for (int y = 0; y < 8; y++){
				std::cout << std::to_string(myVBO.data[debugCount++]) << " ";
			}
			std::cout << "\n";
		}
	}
	glActiveTexture(GL_TEXTURE0);
	numberOfMaterials = scene->mNumMaterials;
	std::vector<int> materialRemap(numberOfMaterials);
	for (int x = 0; x < numberOfMaterials; x++){
		const aiMaterial* material = scene->mMaterials[x];
		int texIndex = 0;
		aiString path;
		material->GetTexture(aiTextureType_DIFFUSE, texIndex, &path);
		if (material->GetTexture(aiTextureType_DIFFUSE, texIndex, &path) == AI_SUCCESS){
			std::string dir = GetDirectoryPath(filePath);
			std::string textureName = path.data;
			std::string fullPath = dir + textureName;
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
	isLoaded = true;
	FinalizeVBO();
	return true;
};
void Model::FinalizeVBO(){
	shader->Use();
	glGenVertexArrays(1, &VAOid);
	glBindVertexArray(VAOid);
	myVBO.BindCreate();
	myVBO.Upload(GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, neededSize, 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, neededSize, (void*)(sizeof(float)* 3));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, neededSize, (void*)(sizeof(float)* 5));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, NUM_BONES_PER_VEREX, GL_FLOAT, GL_FALSE, neededSize, (void*)(sizeof(float)* 8));
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, NUM_BONES_PER_VEREX, GL_FLOAT, GL_FALSE, neededSize, (void*)((sizeof(float)* (8 + NUM_BONES_PER_VEREX))));
	glBindVertexArray(0);
};
void Model::BindModelsVAO(){
	glBindVertexArray(VAOid);
};
void Model::Render(){
	shader->Use();
	if (!isLoaded)
		return;
	BindModelsVAO();
	int size = meshSizes.size();
	for(int x = 0; x < size; x++){
		int iMatIndex = materialIndices[x];
		textures[iMatIndex].Bind();
		glDrawArrays(GL_TRIANGLES, meshStartIndices[x], meshSizes[x]); // Errors Drawing with the shader.
	}
	glBindVertexArray(0);
};
void CopyaiMat(const aiMatrix4x4 *from, glm::mat4 &to) {
	to[0][0] = from->a1; to[1][0] = from->a2;
	to[2][0] = from->a3; to[3][0] = from->a4;
	to[0][1] = from->b1; to[1][1] = from->b2;
	to[2][1] = from->b3; to[3][1] = from->b4;
	to[0][2] = from->c1; to[1][2] = from->c2;
	to[2][2] = from->c3; to[3][2] = from->c4;
	to[0][3] = from->d1; to[1][3] = from->d2;
	to[2][3] = from->d3; to[3][3] = from->d4;
}
void Model::Update(glm::mat4 model){
	shader->Use();
	shader->SetModelAndNormalMatrix("modelMatrix", "normalMatrix", model);
	vector<Matrix4f> Transforms;
	BoneTransform(elapsedTime, Transforms);
	glm::mat4 tempMat4 = glm::mat4(1.0);
	for (int x = 0; x < Transforms.size(); x++){
		char tempName[128];
		memset(tempName, 0, sizeof(tempName));
		sprintf(tempName, "gBones[%d]", x);
		GLuint tempPos = glGetUniformLocation(shader->ID, tempName);
		CopyaiMat(&Transforms[x], tempMat4);
		glUniformMatrix4fv(tempPos, 1, GL_TRUE, glm::value_ptr(tempMat4));
	}
}

void Model::LoadBones(uint MeshIndex, const aiMesh* pMesh, vector<VertexBoneData>& Bones){
	for (uint i = 0; i < pMesh->mNumBones; i++) {
		uint BoneIndex = 0;
		string BoneName(pMesh->mBones[i]->mName.data);

		if (boneMap.find(BoneName) == boneMap.end()) {
			// Allocate an index for a new bone
			BoneIndex = boneCount;
			boneCount++;
			BoneInfo bi;
			boneInfo.push_back(bi);
			boneInfo[BoneIndex].BoneOffset = pMesh->mBones[i]->mOffsetMatrix;
			boneMap[BoneName] = BoneIndex;
		}
		else {
			BoneIndex = boneMap[BoneName];
		}
		for (uint j = 0; j < pMesh->mBones[i]->mNumWeights; j++) {
			uint VertexID = meshStartIndices[MeshIndex] + pMesh->mBones[i]->mWeights[j].mVertexId; //Incorrect
			float Weight = pMesh->mBones[i]->mWeights[j].mWeight;
			Bones[VertexID].AddBoneData(BoneIndex, Weight);
		}
	}
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


void Model::CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim){
	if (pNodeAnim->mNumPositionKeys == 1) {
		Out = pNodeAnim->mPositionKeys[0].mValue;
		return;
	}

	uint PositionIndex = FindPosition(AnimationTime, pNodeAnim);
	uint NextPositionIndex = (PositionIndex + 1);
	assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
	float DeltaTime = (float)(pNodeAnim->mPositionKeys[NextPositionIndex].mTime - pNodeAnim->mPositionKeys[PositionIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiVector3D& Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
	const aiVector3D& End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
	aiVector3D Delta = End - Start;
	Out = Start + Factor * Delta;
}


void Model::CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim){
	// we need at least two values to interpolate...
	if (pNodeAnim->mNumRotationKeys == 1) {
		Out = pNodeAnim->mRotationKeys[0].mValue;
		return;
	}

	uint RotationIndex = FindRotation(AnimationTime, pNodeAnim);
	uint NextRotationIndex = (RotationIndex + 1);
	assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
	float DeltaTime = (float)(pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
	const aiQuaternion& EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
	aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
	Out = Out.Normalize();
}


void Model::CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim){
	if (pNodeAnim->mNumScalingKeys == 1) {
		Out = pNodeAnim->mScalingKeys[0].mValue;
		return;
	}

	uint ScalingIndex = FindScaling(AnimationTime, pNodeAnim);
	uint NextScalingIndex = (ScalingIndex + 1);
	assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
	float DeltaTime = (float)(pNodeAnim->mScalingKeys[NextScalingIndex].mTime - pNodeAnim->mScalingKeys[ScalingIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
	const aiVector3D& End = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
	aiVector3D Delta = End - Start;
	Out = Start + Factor * Delta;
}


void Model::ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const Matrix4f& ParentTransform){
	string NodeName(pNode->mName.data);

	const aiAnimation* pAnimation = scene->mAnimations[0];

	Matrix4f NodeTransformation(pNode->mTransformation);

	const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, NodeName);

	if (pNodeAnim) {
		// Interpolate scaling and generate scaling transformation matrix
		aiVector3D Scaling;
		CalcInterpolatedScaling(Scaling, AnimationTime, pNodeAnim);
		Matrix4f ScalingM;
		ScalingM.Scaling(aiVector3t<float>(Scaling.x, Scaling.y, Scaling.z),ScalingM);//INITSCALING or something TODO

		// Interpolate rotation and generate rotation transformation matrix
		aiQuaternion RotationQ;
		CalcInterpolatedRotation(RotationQ, AnimationTime, pNodeAnim);
		Matrix4f RotationM = Matrix4f(RotationQ.GetMatrix());

		// Interpolate translation and generate translation transformation matrix
		aiVector3D Translation;
		CalcInterpolatedPosition(Translation, AnimationTime, pNodeAnim);
		Matrix4f TranslationM;
		TranslationM.Translation(aiVector3t<float>(Translation.x, Translation.y, Translation.z), TranslationM);

		// Combine the above transformations
		NodeTransformation = TranslationM * RotationM * ScalingM;
	}
	Matrix4f GlobalTransformation = ParentTransform * NodeTransformation;
	if (boneMap.find(NodeName) != boneMap.end()) {
		uint BoneIndex = boneMap[NodeName];
		boneInfo[BoneIndex].FinalTransformation = globalInverseTransform * GlobalTransformation * boneInfo[BoneIndex].BoneOffset;
	}

	for (uint i = 0; i < pNode->mNumChildren; i++) {
		ReadNodeHeirarchy(AnimationTime, pNode->mChildren[i], GlobalTransformation);
	}
}


void Model::BoneTransform(float TimeInSeconds, vector<Matrix4f>& Transforms){
	if (scene->HasAnimations()){ //None of them have animations...
		Matrix4f Identity;
		Identity.IsIdentity();
		float TicksPerSecond = (float)(scene->mAnimations[0]->mTicksPerSecond != 0 ? scene->mAnimations[0]->mTicksPerSecond : 25.0f);
		float TimeInTicks = TimeInSeconds * TicksPerSecond;
		float AnimationTime = fmod(TimeInTicks, (float)scene->mAnimations[0]->mDuration);

		ReadNodeHeirarchy(AnimationTime, scene->mRootNode, Identity);

		Transforms.resize(boneCount);

		for (uint i = 0; i < boneCount; i++) {
			Transforms[i] = boneInfo[i].FinalTransformation;
		}
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

void Model::VertexBoneData::AddBoneData(uint BoneID, float Weight)
{
	for (uint i = 0; i < NUM_BONES_PER_VEREX; i++) {
		if (Weights[i] == 0.0) {
			IDs[i] = BoneID;
			Weights[i] = Weight;
			return;
		}
	}

	// should never get here - more bones than we have space for
	//assert(0);
		
		//...oops
}
