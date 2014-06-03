#version 330

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inCoord;
layout (location = 2) in vec3 inNormal;
layout (location = 3) in vec4 inBoneIDs;
layout (location = 4) in vec4 inWeights;

smooth out vec3 vNormal;
out vec2 vTexCoord;
out vec3 vDirection;
out float ambientIntensity2;
flat out int doOverride2;
out vec4 overrideColor2;

uniform mat4 projMatrix;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;                                                                      
uniform mat4 normalMatrix;
uniform vec3 lightDirection;
uniform int doOverride;
uniform vec4 overrideColor;

uniform float ambientIntensity;

void main()
{
	ambientIntensity2 = ambientIntensity;
	vTexCoord = inCoord;
	vDirection = lightDirection;

	mat4 mMVP = projMatrix*viewMatrix*modelMatrix;

	gl_Position = mMVP * vec4(inPosition,1);
	vec4 vRes = normalMatrix*vec4(inNormal, 0.0);
	vNormal = vRes.xyz;
	
	overrideColor2 = overrideColor;
	doOverride2 = doOverride;
	vNormal.x = inBoneIDs.x;
	vNormal.y = inWeights.x;
}