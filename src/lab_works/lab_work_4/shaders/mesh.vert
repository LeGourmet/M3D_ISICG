#version 450

layout( location = 0 ) in vec3 aVertexPosition;
layout( location = 2 ) in vec3 aVertexNormal;
layout( location = 3 ) in vec2 aVertexTexCoords;
layout( location = 4 ) in vec3 aVertexTangent;
layout( location = 5 ) in vec3 aVertexBitagent;

uniform mat4 uMVPMatrix;
uniform mat4 uMVMatrix;
uniform mat4 uNormalMatrix;

out vec3 normal;
out vec3 fragPos;

void main()
{
	normal = mat3(uNormalMatrix) * aVertexNormal;
	fragPos = ( uMVMatrix * vec4( aVertexPosition, 1.f ) ).xyz;
	gl_Position = uMVPMatrix * vec4( aVertexPosition, 1.f );
}
