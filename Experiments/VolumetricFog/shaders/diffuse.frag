#version 450 core

#define numLights 2
layout (location = 10) uniform vec3 lightDirections[numLights];
layout (location = 14) uniform vec4 lightColors[numLights];
layout (binding = 0) uniform sampler2D tex0;
uniform sampler2D shadowMaps[numLights];

in vec2 texCoord;
in vec3 worldPos;
in vec3 worldNormal;
in vec4 lightSpacePos[numLights];

out vec4 color;

void main()
{
	vec3 diffuseColor = vec3(0.0f);
	for (int i = 0; i < numLights; ++i)	
	{
		vec3 projCoords = lightSpacePos[i].xyz / lightSpacePos[i].w;
		if (projCoords.z <= 1.0) 
		{
			projCoords = projCoords * 0.5 + 0.5;
			float currentDepth = projCoords.z;
			float closestDepth = texture(shadowMaps[i], projCoords.xy).r; 
			float bias = 0.005;
			float shadow = currentDepth - bias > closestDepth ? 0.0 : 1.0;  

			float diffuse = max(dot(worldNormal, -lightDirections[i]), 0.0);
			diffuseColor += diffuse * lightColors[i].rgb * lightColors[i].a * shadow;
		}
	}
	
	color.rgb += (diffuseColor + vec3(0.1f)) * vec3(texture(tex0, texCoord));
} 