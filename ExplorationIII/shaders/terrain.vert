#version 330

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inCoord;
layout (location = 2) in vec3 inNormal;

smooth out vec3 vNormal;
out vec2 vTexCoord;
out vec3 vDirection;
out float ambientIntensity2;
out vec3 vWorldPos;
out float fRenderHeight2;
out float fMaxTextureU2;
out float fMaxTextureV2;

uniform mat4 projMatrix;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;                                                                      
uniform vec3 lightDirection;
uniform float ambientIntensity;
uniform float fRenderHeight;
uniform float fMaxTextureU;
uniform float fMaxTextureV;

uniform mat4 HeightmapScaleMatrix;

void main()
{
	ambientIntensity2 = ambientIntensity;
	vTexCoord = inCoord;
	vDirection = lightDirection;
	fRenderHeight2 = fRenderHeight;
	fMaxTextureU2 = fMaxTextureU;
	fMaxTextureV2 = fMaxTextureV;

	vec4 inPositionScaled = HeightmapScaleMatrix*vec4(inPosition, 1.0);
	mat4 mMVP = projMatrix*viewMatrix*modelMatrix;
	gl_Position = mMVP*inPositionScaled;
	
	vNormal = inNormal;
	vec4 vWorldPosLocal = modelMatrix*inPositionScaled;
	vWorldPos = vWorldPosLocal.xyz;
}