#version 330

smooth in vec3 vNormal;
in vec2 vTexCoord;
in float ambientIntensity2;
in vec3 vDirection;
flat in int doOverride2;
in vec4 overrideColor2;

out vec4 outputColor;

uniform sampler2D gSampler;

void main(){
	outputColor = vec4(0.0,0.0,1.0,0.25);//overrideColor2;
}