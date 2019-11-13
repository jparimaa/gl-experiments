#version 450 core
layout (location = 0) in vec3 position;

layout (location = 0) uniform mat4 lightSpaceVP;
layout (location = 1) uniform mat4 modelMatrix;

void main()
{
    gl_Position = lightSpaceVP * modelMatrix * vec4(position, 1.0);
}