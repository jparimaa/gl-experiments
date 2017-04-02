#version 450 core

layout (location = 1) uniform float time;
layout (binding = 2) uniform sampler2D tex0;

in vec2 texCoord;

out vec4 color;

void main()
{
	float f = sin(time);
	color = vec4(texture(tex0, texCoord).gb, f, 1.0f);
} 