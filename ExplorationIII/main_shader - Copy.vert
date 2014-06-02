#version 330

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inCoord;
layout (location = 2) in vec3 inNormal;

out vec3 vNormal;
out vec2 vTexCoord;
out vec3 vWorldPos;
out vec4 vEyeSpacePos;
out vec3 EyeDirection_cameraspace;
out vec3 LightDirection_cameraspace;
out vec3 LightPosition_worldspace2;
out float ambientIntensity2;

uniform vec3 LightPosition_worldspace;
uniform mat4 projMatrix;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;                                                                      
uniform mat4 normalMatrix;
uniform float ambientIntensity;

void main()
{
	ambientIntensity2 = ambientIntensity;
	LightPosition_worldspace2 = LightPosition_worldspace;
	mat4 mMV = viewMatrix*modelMatrix;  
	mat4 mMVP = projMatrix*viewMatrix*modelMatrix;

	vTexCoord = inCoord;

	mat4 MVP = projMatrix * viewMatrix * modelMatrix; // THIS MAY CAUSE LAG I HAVE NO IDEA.
	
	vEyeSpacePos = mMV*vec4(inPosition, 1.0);
	gl_Position = MVP * vec4(inPosition,1);//mMVP*vec4(inPosition, 1.0);

	vNormal = (viewMatrix * modelMatrix * vec4(inNormal,0)).xyz;//(normalMatrix*vec4(inNormal, 1.0)).xyz;
	vWorldPos = (modelMatrix*vec4(inPosition, 1.0)).xyz;

	// Vector that goes from the vertex to the camera, in camera space.
	// In camera space, the camera is at the origin (0,0,0).
	vec3 vertexPosition_cameraspace = ( viewMatrix * modelMatrix * vec4(inPosition,1)).xyz;
	EyeDirection_cameraspace = vec3(0,0,0) - vertexPosition_cameraspace;
	
	// Vector that goes from the vertex to the light, in camera space. M is ommited because it's identity.
	vec3 LightPosition_cameraspace = ( viewMatrix * vec4(LightPosition_worldspace,1)).xyz;
	LightDirection_cameraspace = LightPosition_cameraspace + EyeDirection_cameraspace;
}