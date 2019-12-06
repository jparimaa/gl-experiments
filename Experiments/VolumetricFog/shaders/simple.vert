#version 450 core
layout (location = 0) in vec3 position;

layout (location = 0) uniform mat4 MVP;

void main()
{
	gl_Position = MVP * vec4(position, 1.0);
}