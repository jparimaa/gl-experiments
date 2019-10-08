#version 450 core
layout (location = 0) uniform int width;
layout (location = 1) uniform int height;
layout (location = 2) uniform float time;

layout (binding = 6) writeonly uniform uimage2D tex;

void main()
{
	ivec2 i = ivec2(gl_VertexID % width, gl_VertexID / width);
	uvec4 color = uvec4(i / 2, 255, 0);
	imageStore(tex, i, color);
}