#version 440 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 tex;
layout (location = 2) in vec3 normal;

out vec2 texCoord;
out vec3 normal_World;

const int NUM_PRIMITIVES = 100;

layout (std140, binding = 1) uniform transformationMatrices {
	mat4 modelMatrices[NUM_PRIMITIVES];
	mat4 mvpMatrices[NUM_PRIMITIVES];
};

void main()
{
	gl_Position = mvpMatrices[gl_InstanceID] * vec4(position, 1.0);
	texCoord = tex;
	normal_World = normalize(mat3(modelMatrices[gl_InstanceID]) * normal);
}