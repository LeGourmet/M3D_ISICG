#version 450

layout( location = 0 ) out vec4 fragColor;
layout( binding = 0 ) uniform sampler2D uAmbientMap;
layout( binding = 1 ) uniform sampler2D uDiffuseMap;
layout( binding = 2 ) uniform sampler2D uSpecularMap;
layout( binding = 3 ) uniform sampler2D uShininessMap;
layout( binding = 4 ) uniform sampler2D uNormalMap;

uniform vec3 uAmbient;
uniform vec3 uDiffuse;
uniform vec3 uSpecular;
uniform float uShininess;

uniform bool uHasAmbientMap;
uniform bool uHasDiffuseMap;
uniform bool uHasSpecularMap;
uniform bool uHasShininessMap;
uniform bool uHasNormalMap;

in vec2 uv;
in vec3 normal;
in vec3 fragPos;
in vec3 fragPosT;

void main()
{
	if(uHasDiffuseMap && texture2D(uDiffuseMap,uv).w<0.5f){discard;}

	vec3 Li = normalize(uHasNormalMap ? fragPosT : fragPos);
	vec3 Lo = Li;
	vec3 N = normalize(uHasNormalMap ? (texture2D(uNormalMap,uv)*2.f-1.f).xyz : normal);
	if(dot(N,Lo)<0){N *= -1.f;}

	float Ia = 1.f;
	float Id = 1.f*(max(0.f,dot(N,Li)));
	float Is = 1.f*pow(max(0.f,dot(Lo,reflect(-Li, N))), (uHasShininessMap ? texture2D(uShininessMap,uv).x : uShininess) );
	float Il = 1.f;

	vec3 colorL = vec3(1.f);
	vec3 colorA = (uHasAmbientMap ? texture2D(uAmbientMap,uv).xyz : uAmbient);
	vec3 colorD = (uHasDiffuseMap ? texture2D(uDiffuseMap,uv).xyz : uDiffuse);
	vec3 colorS = (uHasSpecularMap ? texture2D(uSpecularMap,uv).xxx : uSpecular);
		
	fragColor = vec4(Ia*colorA + ( Id*colorD + Is*colorS) * Il*colorL, uHasDiffuseMap ? texture2D(uDiffuseMap,uv).w : 1.f);
}
