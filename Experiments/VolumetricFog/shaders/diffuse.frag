#version 450 core

#define numLights 4
layout (location = 10) uniform vec3 lightPositions[numLights];
layout (location = 14) uniform vec4 lightColors[numLights];
layout (binding = 0) uniform sampler2D tex0;

in vec2 texCoord;
in vec3 worldPos;
in vec3 worldNormal;

out vec4 color;

void main()
{
	for (int i = 0; i < 4; ++i)	
	{
		vec3 toLightVec = lightPositions[i] - worldPos;

		vec3 dir = normalize(toLightVec);		
		float diffuse = max(dot(worldNormal, dir), 0.0);

		float distance = length(toLightVec);

		color += diffuse * lightColors[i] * lightColors[i].a * (1.0f / distance);
	}
	color += vec4(0.1f) * vec4(texture(tex0, texCoord));
} 