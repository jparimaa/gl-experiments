#version 440 core

uniform float time;

out vec4 color;

void main()
{
    color = vec4(sin(time), cos(time), 0.2f, 1.0f);
} 