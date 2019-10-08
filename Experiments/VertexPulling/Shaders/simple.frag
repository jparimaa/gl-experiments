#version 450 core
layout (binding = 0) uniform sampler2D tex0;

in vec2 texCoord;
in vec3 normal_World;

out vec4 color;

void main()
{
	vec3 lightDir = normalize(-vec3(-1.0f, -1.0f, 1.0f));
	float diff = max(dot(normal_World, lightDir), 0.0);
	vec4 texColor = vec4(texture(tex0, texCoord).rgb, 1.0f);
	color = 0.1 * texColor + diff * texColor;
} 