#version 450 core

layout (location = 0) uniform int width;
layout (location = 1) uniform int height;
layout (location = 2) uniform float time;
layout (binding = 6, rgba8ui) readonly uniform uimage2D tex;

in vec2 uv;
out vec4 color;

void main() 
{
	vec4 s = imageLoad(tex, ivec2(float(width) * uv.x, float(height) * uv.y));
	color = (1.0 / 255.0) * s;
	color.r += sin(time);
	color.g *= sin(time) + cos(time);
	color.b += cos(time);
}