#version 440 core
#extension GL_ARB_bindless_texture : require

layout(binding = 0) uniform Textures
{
	uvec2 tex0;
};

in vec2 uv;

out vec4 color;

void main()
{
	color = texture(sampler2D(tex0), uv);
} 