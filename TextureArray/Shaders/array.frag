#version 440 core

layout (binding = 0) uniform sampler2DArray texArray;
layout (location = 0) uniform int layer;

in vec2 uv;

out vec4 color;

void main()
{
	color = texture(texArray, vec3(uv.x, uv.y, layer));
} 