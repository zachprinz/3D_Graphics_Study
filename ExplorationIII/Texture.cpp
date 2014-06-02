#include "texture.h"

#include <FI/FreeImage.h>

Texture::Texture(){
	mipMapsGenerated = false;
}

void Texture::Create(int a_iWidth, int a_iHeight, GLenum format){
	std::cout << "Created 1" << std::endl;

	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	if (format == GL_RGBA || format == GL_BGRA)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, a_iWidth, a_iHeight, 0, format, GL_UNSIGNED_BYTE, NULL);
	// We must handle this because of internal format parameter
	else if (format == GL_RGB || format == GL_BGR)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, a_iWidth, a_iHeight, 0, format, GL_UNSIGNED_BYTE, NULL);
	else
		glTexImage2D(GL_TEXTURE_2D, 0, format, a_iWidth, a_iHeight, 0, format, GL_UNSIGNED_BYTE, NULL);

	glGenSamplers(1, &samplerID);
}

void Texture::CreateFromData(BYTE* bData, int a_iWidth, int a_iHeight, int a_iBPP, GLenum format, bool bGenerateMipMaps){
	std::cout << "Created 2" << std::endl;
	// Generate an OpenGL texture ID for this texture
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	std::cout << "Texture: " << std::to_string(id) << std::endl;
	if (format == GL_RGBA || format == GL_BGRA)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, a_iWidth, a_iHeight, 0, format, GL_UNSIGNED_BYTE, bData);
	// We must handle this because of internal format parameter
	else if (format == GL_RGB || format == GL_BGR)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, a_iWidth, a_iHeight, 0, format, GL_UNSIGNED_BYTE, bData);
	else
		glTexImage2D(GL_TEXTURE_2D, 0, format, a_iWidth, a_iHeight, 0, format, GL_UNSIGNED_BYTE, bData);
	if (bGenerateMipMaps)glGenerateMipmap(GL_TEXTURE_2D);
	glGenSamplers(1, &samplerID);

	path = "";
	mipMapsGenerated = bGenerateMipMaps;
	width = a_iWidth;
	height = a_iHeight;
	BPP = a_iBPP;
}

bool Texture::Load(std::string a_path, bool bGenerateMipMaps){
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	FIBITMAP* dib(0);

	fif = FreeImage_GetFileType(a_path.c_str(), 0); // Check the file signature and deduce its format

	if (fif == FIF_UNKNOWN) // If still unknown, try to guess the file format from the file extension
		fif = FreeImage_GetFIFFromFilename(a_path.c_str());

	if (fif == FIF_UNKNOWN) // If still unknown, return failure
		return false;

	if (FreeImage_FIFSupportsReading(fif)) // Check if the plugin has reading capabilities and load the file
		dib = FreeImage_Load(fif, a_path.c_str());
	if (!dib)
		return false;

	BYTE* bDataPointer = FreeImage_GetBits(dib); // Retrieve the image data

	// If somehow one of these failed (they shouldn't), return failure
	if (bDataPointer == NULL || FreeImage_GetWidth(dib) == 0 || FreeImage_GetHeight(dib) == 0)
		return false;

	GLenum format;
	int bada = FreeImage_GetBPP(dib);
	if (FreeImage_GetBPP(dib) == 32)format = GL_RGBA;
	if (FreeImage_GetBPP(dib) == 24)format = GL_BGR;
	if (FreeImage_GetBPP(dib) == 8)format = GL_LUMINANCE;
	CreateFromData(bDataPointer, FreeImage_GetWidth(dib), FreeImage_GetHeight(dib), FreeImage_GetBPP(dib), format, bGenerateMipMaps);

	FreeImage_Unload(dib);

	path = a_path;
	std::cout << "Loaded" << std::endl;

	return true; // Success
}

void Texture::SetSamplerParameter(GLenum parameter, GLenum value){
	glSamplerParameteri(samplerID, parameter, value);
}

void Texture::SetFiltering(int a_mag, int a_min){
	glBindSampler(0, samplerID);

	// Set magnification filter
	if (a_mag == TEXTURE_FILTER_MAG_NEAREST)
		glSamplerParameteri(samplerID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	else if (a_mag == TEXTURE_FILTER_MAG_BILINEAR)
		glSamplerParameteri(samplerID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Set minification filter
	if (a_min == TEXTURE_FILTER_MIN_NEAREST)
		glSamplerParameteri(samplerID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	else if (a_min == TEXTURE_FILTER_MIN_BILINEAR)
		glSamplerParameteri(samplerID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	else if (a_min == TEXTURE_FILTER_MIN_NEAREST_MIPMAP)
		glSamplerParameteri(samplerID, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	else if (a_min == TEXTURE_FILTER_MIN_BILINEAR_MIPMAP)
		glSamplerParameteri(samplerID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	else if (a_min == TEXTURE_FILTER_MIN_TRILINEAR)
		glSamplerParameteri(samplerID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	min = a_min;
	mag = a_mag;
}

void Texture::Bind(int iTextureUnit){
	glActiveTexture(GL_TEXTURE0 + iTextureUnit);
	glBindTexture(GL_TEXTURE_2D, id);
	glBindSampler(iTextureUnit, samplerID);
}

void Texture::Bind(int iTextureUnit, GLuint samplerID2){
	glActiveTexture(GL_TEXTURE0 + iTextureUnit);
	glBindTexture(GL_TEXTURE_2D, id);
	glBindSampler(iTextureUnit, samplerID2);
}

void Texture::Delete(){
	glDeleteSamplers(1, &samplerID);
	glDeleteTextures(1, &id);
}

int Texture::GetMinFilter(){
	return min;
}

int Texture::GetMagFilter(){
	return mag;
}

int Texture::GetWidth(){
	return width;
}

int Texture::GetHeight(){
	return height;
}

int Texture::GetBPP(){
	return BPP;
}

GLuint Texture::GetID(){
	return id;
}

std::string Texture::GetPath(){
	return path;
}

/*bool Texture::Reload(){
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	FIBITMAP* dib(0);

	fif = FreeImage_GetFileType(path.c_str(), 0); // Check the file signature and deduce its format

	if (fif == FIF_UNKNOWN) // If still unknown, try to guess the file format from the file extension
		fif = FreeImage_GetFIFFromFilename(path.c_str());

	if (fif == FIF_UNKNOWN) // If still unknown, return failure
		return false;

	if (FreeImage_FIFSupportsReading(fif)) // Check if the plugin has reading capabilities and load the file
		dib = FreeImage_Load(fif, path.c_str());
	if (!dib)
		return false;

	BYTE* bDataPointer = FreeImage_GetBits(dib); // Retrieve the image data

	// If somehow one of these failed (they shouldn't), return failure
	if (bDataPointer == NULL || FreeImage_GetWidth(dib) == 0 || FreeImage_GetHeight(dib) == 0)
		return false;

	GLenum format;
	int bada = FreeImage_GetBPP(dib);
	if (FreeImage_GetBPP(dib) == 32)format = GL_RGBA;
	if (FreeImage_GetBPP(dib) == 24)format = GL_BGR;
	if (FreeImage_GetBPP(dib) == 8)format = GL_LUMINANCE;

	glBindTexture(GL_TEXTURE_2D, id);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, iWidth, iHeight, format, GL_UNSIGNED_BYTE, bDataPointer);
	if (bMipMapsGenerated)glGenerateMipmap(GL_TEXTURE_2D);

	FreeImage_Unload(dib);

	return true; // Success
}*/

Texture tTextures[NUMTEXTURES];

/*void LoadAllTextures(){
	// Load textures

	std::string sTextureNames[] = { "sand_grass_02.jpg" };

	FOR(i, NUMTEXTURES)
	{
		tTextures[i].LoadTexture2D("data\\textures\\" + sTextureNames[i], true);
		tTextures[i].SetFiltering(TEXTURE_FILTER_MAG_BILINEAR, TEXTURE_FILTER_MIN_BILINEAR_MIPMAP);
	}
}*/