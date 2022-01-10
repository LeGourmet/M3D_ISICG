#version 450

layout( location = 0 ) in vec3 aVertexPosition;
layout( location = 2 ) in vec3 aVertexNormal;
layout( location = 3 ) in vec2 aVertexTexCoords;
layout( location = 4 ) in vec3 aVertexTangent;
layout( location = 5 ) in vec3 aVertexBitangent;

uniform mat4 uMVPMatrix;
uniform mat4 uMVMatrix;
uniform mat4 uNormalMatrix;

out vec2 uv;
out vec3 normal;
out vec3 fragPos;
out vec3 fragPosT;

void main()
{
	uv = aVertexTexCoords;
	normal = (uNormalMatrix * vec4(aVertexNormal,1.f)).xyz;

	vec3 N	 = normalize( (uMVMatrix * vec4(aVertexNormal,0.f)).xyz );
	vec3 T	 = normalize( (uMVMatrix * vec4(aVertexTangent,0.f)).xyz );
	vec3 B	 = normalize( (uMVMatrix * vec4(aVertexBitangent,0.f)).xyz );
	mat3 inv_TBN = inverse(mat3(T,B,N));

	fragPos = (uMVMatrix * vec4( aVertexPosition, 1.f ) ).xyz;
	fragPosT = inv_TBN * fragPos;
	gl_Position = uMVPMatrix * vec4( aVertexPosition, 1.f );
}
