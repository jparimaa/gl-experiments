#version 440 core
layout(location = 0) out vec4 outColor;

layout (location = 30) uniform sampler2D renderedTex;

in vec2 texCoord;

void main()
{
    outColor = texture(renderedTex, texCoord);
}
