#version 330

uniform sampler2D gSampler[5];

in float fRenderHeight2;
in float fMaxTextureU2;
in float fMaxTextureV2;

smooth in vec3 vNormal;
in vec2 vTexCoord;
in float ambientIntensity2;
in vec3 vDirection;
in vec3 vWorldPos;

out vec4 outputColor;

void main(){
	vec3 vNormalized = normalize(vNormal);

	vec4 vTexColor = vec4(0.0);

	float fScale = (vWorldPos.y+10.0)/fRenderHeight2;

	const float fRange1 = 0.15f;
	const float fRange2 = 0.3f;
	const float fRange3 = 0.65f;
	const float fRange4 = 0.85f;

	if(fScale >= 0.0 && fScale <= fRange1){
		vTexColor = texture2D(gSampler[0], vTexCoord);
	}
	else {
		if(fScale <= fRange2){
			fScale -= fRange1;
			fScale /= (fRange2-fRange1);
			float fScale2 = fScale;
			fScale = 1.0-fScale;
			vTexColor += texture2D(gSampler[0], vTexCoord)*fScale;
			vTexColor += texture2D(gSampler[1], vTexCoord)*fScale2;
		}
		else {
			if(fScale <= fRange3){
				vTexColor = texture2D(gSampler[1], vTexCoord);
			}
			else {
				if(fScale <= fRange4){
					fScale -= fRange3;
					fScale /= (fRange4-fRange3);

					float fScale2 = fScale;
					fScale = 1.0-fScale; 

					vTexColor += texture2D(gSampler[1], vTexCoord)*fScale;
					vTexColor += texture2D(gSampler[2], vTexCoord)*fScale2;
				}
				else 
					vTexColor = texture2D(gSampler[2], vTexCoord);
				}
		}
	}
	vec2 vPathCoord = vec2(vTexCoord.x/fMaxTextureU2, vTexCoord.y/fMaxTextureV2);
	vec4 vPathIntensity = texture2D(gSampler[4], vPathCoord);
	fScale = vPathIntensity.x;

	vec4 vPathColor = texture2D(gSampler[3], vTexCoord);
	vec4 vFinalTexColor = vTexColor;//(fScale*vTexColor)+((1.0-fScale)*vPathColor);

	vec4 vMixedColor = vFinalTexColor * vec4(1.0,1.0,1.0,1.0);

	vec3 lightColor = vec3(1.0f,1.0f,1.0f);

	float fDiffuseIntensity = max(0.0, dot(normalize(vNormal), -vDirection)); 
	outputColor = vMixedColor;//*vec4(lightColor*(ambientIntensity2+fDiffuseIntensity ), 0.0);

}