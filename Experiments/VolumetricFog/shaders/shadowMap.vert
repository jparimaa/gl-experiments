#version 450 core
layout (location = 0) in vec3 position;

layout (location = 1) uniform mat4 modelMatrix;
layout (location = 2) uniform mat4 lightSpaceVP;

void main()
{
    gl_Position = lightSpaceVP * modelMatrix * vec4(position, 1.0);
}