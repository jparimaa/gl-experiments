#version 450 core

layout (binding = 0) uniform sampler2D depth;
layout (binding = 1) uniform sampler3D cumulativeScatteringData;

layout (location = 1) uniform mat4 inverseProjection;

in vec2 texCoord;

out vec4 color;

void main()
{
	float depth = texture(depth, texCoord).r;
    float z = depth * 2.0 - 1.0;

	vec4 viewportPos = vec4(texCoord.x * 2 - 1, 
                            texCoord.y * 2 - 1,
                            z, 
							1); 

    vec4 viewPos = inverseProjection * viewportPos;
    viewPos /= viewPos.w;
	
	float depthCoordinate = -viewPos.z / 49.5f;

	color = 0.3f * texture(cumulativeScatteringData, vec3(texCoord.x, texCoord.y, depthCoordinate));
} 