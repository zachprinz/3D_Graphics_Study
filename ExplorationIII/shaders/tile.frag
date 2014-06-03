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
	// Defaults
	//if(doOverride2 > 0){
	//	outputColor = vec4(0.0,0.0,1.0,1.0);//overrideColor2;
	//}
	//else {
		vec4 vTexColor = texture2D(gSampler, vTexCoord);
		vec3 lightColor = vec3(1.0f,1.0f,1.0f);
		float fDiffuseIntensity = max(0.0, dot(normalize(vNormal), -vDirection)); 
		if(vTexColor.a < 0.1)
			discard;
		else
			outputColor = vTexColor*vec4(lightColor*(ambientIntensity2+fDiffuseIntensity), 1.0);
	//}
}