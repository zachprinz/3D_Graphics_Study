#include "VBO.h"

VBO::VBO(){
	dataUploaded = false;
	id = 0;
	count = 0;
}

void VBO::Create(int a_iSize){
	data = (float*)malloc(sizeof(float)*(a_iSize));
	size = a_iSize;
	currentSize = 0;
	debugCount = 0;
	glGenBuffers(1, &id);
}

void VBO::Delete(){
	glDeleteBuffers(1, &id);
	dataUploaded = false;
	free(data);
}

void* VBO::MapBufferToMemory(int iUsageHint){
	if (!dataUploaded)return NULL;
	void* ptrRes = glMapBuffer(type, iUsageHint);
	return ptrRes;
}

void* VBO::MapSubBufferToMemory(int iUsageHint, GLuint uiOffset, GLuint uiLength){
	if (!dataUploaded)return NULL;
	void* ptrRes = glMapBufferRange(type, uiOffset, uiLength, iUsageHint);
	return ptrRes;
}

void VBO::Unmap(){
	glUnmapBuffer(type);
}
void VBO::Bind(int a_type)
{
	type = a_type;
	glBindBuffer(type, id);
}
void VBO::BindCreate(int a_type)
{
	glGenBuffers(1, &id);
	type = a_type;
	glBindBuffer(type, id);
}

void VBO::Upload(GLuint iDrawingHint){
	glBufferData(type, sizeof(float)* count, data, GL_STATIC_DRAW);// iDrawingHint);
	dataUploaded = true;
	free(data);
}

void VBO::AddData(float* ptrData){

}

void* VBO::GetDataPointer(){
	if (dataUploaded)return NULL;
	return &data;
}

GLuint VBO::GetID(){
	return id;
}

int VBO::GetSize(){
	return currentSize;
}
void VBO::Add(std::vector< std::vector< glm::vec3> > data[3], glm::vec2 size){
	for (int i = 0; i < size.x; i++)
	{
		for (int j = 0; j < size.y; j++)
		{
			debugCount++;
			this->data[count++] = (float)(data[0][i][j].x);
			this->data[count++] = (float)(data[0][i][j].y);
			this->data[count++] = (float)(data[0][i][j].z);
			this->data[count++] = (float)(data[1][i][j].x);
			this->data[count++] = (float)(data[1][i][j].y);
			this->data[count++] = (float)(data[2][i][j].x);
			this->data[count++] = (float)(data[2][i][j].y);
			this->data[count++] = (float)(data[2][i][j].z);
		}
	}
};
