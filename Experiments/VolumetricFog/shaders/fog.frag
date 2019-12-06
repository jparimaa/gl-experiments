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
    
    float halfWidthAtZ = tan((hfov / 2.0f) * toRadians) * -viewPos.z;
    float halfHeightAtZ = tan((vfov / 2.0f) * toRadians) * -viewPos.z;

    float x = (viewPos.x + halfWidthAtZ) / (2.0f * halfWidthAtZ);
    float y = (viewPos.y + halfHeightAtZ) / (2.0f * halfHeightAtZ);

	color = 0.3f * texture(cumulativeScatteringData, vec3(x, y, depthCoordinate));
} 