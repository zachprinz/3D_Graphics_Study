#version 330

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inCoord;
layout (location = 2) in vec3 inNormal;
layout (location = 3) in vec4 BoneIDs;
layout (location = 4) in vec4 Weights;

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
uniform mat4 gBones[100];

uniform float ambientIntensity;

void main()
{
	if(Weights[0] != 0.0 || Weights[1] != 0.0 || Weights[2] != 0.0 || Weights[3] != 0.0){
		mat4 BoneTransform = gBones[int(BoneIDs[0])] * Weights[0];
		BoneTransform     += gBones[int(BoneIDs[1])] * Weights[1];
		BoneTransform     += gBones[int(BoneIDs[2])] * Weights[2];
		BoneTransform     += gBones[int(BoneIDs[3])] * Weights[3];
		
		ambientIntensity2 = ambientIntensity;
		vTexCoord = inCoord;
		vDirection = lightDirection;

		vec4 PosL = BoneTransform * vec4(inPosition, 1.0);
		
		mat4 mMVP = projMatrix*viewMatrix*(modelMatrix);

		gl_Position = mMVP * PosL;
		vec4 vRes = normalMatrix*vec4(inNormal, 0.0);
		vNormal = vRes.xyz;
		
		overrideColor2 = overrideColor;
		doOverride2 = doOverride;
	}
	else{
		ambientIntensity2 = ambientIntensity;
		vTexCoord = inCoord;
		vDirection = lightDirection;

		mat4 mMVP = projMatrix*viewMatrix*(modelMatrix);

		gl_Position = mMVP * vec4(inPosition,1);
		vec4 vRes = normalMatrix*vec4(inNormal, 0.0);
		vNormal = vRes.xyz;
		
		overrideColor2 = overrideColor;
		doOverride2 = doOverride;
	}
}