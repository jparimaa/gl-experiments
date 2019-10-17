#version 450 core

layout (binding = 0) uniform sampler2D albedo;
layout (binding = 1) uniform sampler2D projectionTexture;

in vec2 texCoord;
in vec2 projectionUvPosition;

out vec4 color;

void main()
{
	vec2 projectionSamplePos = (projectionUvPosition + 1.0) / 2.0;
	float scale = 2.0;
	projectionSamplePos /= scale;
	color = texture(albedo, texCoord) + 0.5 * texture(projectionTexture, projectionSamplePos);
} 