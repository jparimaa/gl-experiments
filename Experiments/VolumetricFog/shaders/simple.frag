#version 450 core

layout (location = 2) uniform vec3 lightColor;

out vec4 color;

void main()
{
	color.rgb = lightColor;
} 