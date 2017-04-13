#version 450 core
layout (location = 0) in vec3 position;

out vec3 positionVS;

void main()
{
	positionVS = position;
}