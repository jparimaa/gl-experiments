#version 450 core

layout (location = 0) in vec2 position;

layout (location = 0) uniform float time;

out vec2 outPosition;

void main()
{
	float sign = (float(gl_VertexID % 2) - 0.5f) * 0.1f;
	outPosition = vec2(position.x + (sin(time) * sign), position.y + (cos(time) * sign));
}