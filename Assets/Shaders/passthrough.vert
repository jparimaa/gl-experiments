#version 440 core
layout (location = 1) in vec3 position;

out vec2 texCoord;

void main()
{
    texCoord = (position.xy + vec2(1,1)) / 2.0;
    gl_Position = vec4(position, 1.0);
}
