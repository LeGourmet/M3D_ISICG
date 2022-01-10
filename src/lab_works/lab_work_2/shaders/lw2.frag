#version 450

layout(location = 0) out vec4 fragColor;

in vec3 color;

uniform float uLux;

void main() {
	fragColor = vec4(color*uLux,1.f);
}
