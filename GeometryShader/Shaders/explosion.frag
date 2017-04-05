#version 450 core

layout (binding = 3) uniform sampler2D tex0;

in vec2 texGS;
in vec3 normalWorldGS;

out vec4 color;

void main()
{
	vec3 lightDir = normalize(-vec3(0.0f, -1.0f, 1.0f));
	float diff = max(dot(normalWorldGS, lightDir), 0.0);
	color = diff * vec4(texture(tex0, texGS).rgb, 1.0f);
} 