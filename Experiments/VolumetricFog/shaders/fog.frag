#version 450 core

#define toRadians 0.0174533

layout (binding = 0) uniform sampler2D depth;
layout (binding = 1) uniform sampler3D cumulativeScatteringData;

layout (location = 1) uniform mat4 inverseProjection;
layout (location = 2) uniform float hfov;
layout (location = 3) uniform float vfov;

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
	
	float depthCoordinate = -viewPos.z / 50.0f;

    // Should be: lighting * fog.a + fog.rgb
	color = texture(cumulativeScatteringData, vec3(texCoord.x, texCoord.y, depthCoordinate));
} 