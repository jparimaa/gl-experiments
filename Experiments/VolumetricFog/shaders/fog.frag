#version 450 core

layout (std430, binding = 0) readonly buffer cumulativeScatteringData
{
    vec4 cumulativeScattering[];
};

layout (binding = 0) uniform sampler2D depth;
layout (location = 1) uniform mat4 inverseProjection;

in vec2 texCoord;

out vec4 color;

vec4 getDensity(vec3 viewPos)
{
	return vec4(0.5f);
}

void main()
{
	float depth = texture(depth, texCoord).r;

	vec4 viewportPos = vec4(texCoord.x * 2 - 1, 
                            (1 - texCoord.y) * 2 - 1,
                            depth, 
							1); 

    vec4 viewPos = inverseProjection * viewportPos;
    viewPos /= viewPos.w;
	color = getDensity(viewPos.xyz);
} 