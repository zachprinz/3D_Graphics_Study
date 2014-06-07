#ifndef TEXTURE_H
#define TEXTURE_H

#include "GL/glew.h"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <cstring>
#include <vector>
#include <sstream>
#include <queue>
#include <map>
#include <set>
#include <gl/glew.h>
#include <glm/glm.hpp>

#include <FI/FreeImage.h>


enum TextureFiltering{
	TEXTURE_FILTER_MAG_NEAREST = 0, // Nearest criterion for magnification
	TEXTURE_FILTER_MAG_BILINEAR, // Bilinear criterion for magnification
	TEXTURE_FILTER_MIN_NEAREST, // Nearest criterion for minification
	TEXTURE_FILTER_MIN_BILINEAR, // Bilinear criterion for minification
	TEXTURE_FILTER_MIN_NEAREST_MIPMAP, // Nearest criterion for minification, but on closest mipmap
	TEXTURE_FILTER_MIN_BILINEAR_MIPMAP, // Bilinear criterion for minification, but on closest mipmap
	TEXTURE_FILTER_MIN_TRILINEAR, // Bilinear criterion for minification on two closest mipmaps, then averaged
};

#define NUMTEXTURES 1

class Texture{
public:
	Texture();
	void Create(int width, int height, GLenum format);
	void CreateFromData(BYTE* bData, int a_iWidth, int a_iHeight, int a_iBPP, GLenum format, bool bGenerateMipMaps);
	bool Load(std::string path, bool generateMipMaps);
	void Bind(int texture = 0);
	void Bind(int texture, GLuint samplerID);
	void SetFiltering(int mag, int min);
	void SetSamplerParameter(GLenum param, GLenum value);
	int GetWidth();
	int GetHeight();
	int GetBPP();
	int GetMinFilter();
	int GetMagFilter();
	GLuint GetID();
	std::string GetPath();
	void Delete();
	GLuint samplerID;
	bool created;
private:
	int width, height, BPP;
	GLuint id;
	bool mipMapsGenerated;
	int min, mag;
	std::string path;
};
#endif