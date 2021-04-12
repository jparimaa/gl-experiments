#version 450 core


layout (binding = 0) uniform sampler2D myImage;

in vec2 texCoord;

out vec4 color;

void main()
{
color = texture(myImage, texCoord);
} 