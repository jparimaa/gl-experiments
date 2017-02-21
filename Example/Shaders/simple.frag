#version 440 core

uniform float time;
uniform sampler2D tex0;

in vec2 texCoord;

out vec4 color;

void main()
{
	float f = sin(time);
	color = vec4(texture(tex0, texCoord).gb, f, 1.0f);
} 