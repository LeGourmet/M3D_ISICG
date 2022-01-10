#version 450

layout( location = 0 ) out vec4 fragColor;

uniform vec3 uAmbient;
uniform vec3 uDiffuse;
uniform vec3 uSpecular;
uniform float uShininess;

in vec3 normal;
in vec3 fragPos;

void main()
{
	vec3 Li = normalize(-fragPos);
	vec3 Lo = normalize(-fragPos);
	vec3 n = normalize(normal);
	if(dot(n,Lo)<0){n *= -1.f;}

	vec3 luxColor = vec3(1.f);

	float Ia = 1.f;
	float Id = 1.f*(max(0.f,dot(n,Li)));
	float Is = 1.f*pow(max(0.f,dot(Lo,reflect(-Li, n))),uShininess);
	float Il = 1.f;

	fragColor = vec4(Ia*uAmbient + (Id*uDiffuse + Is*uSpecular)* Il*luxColor, 1.f);
}
