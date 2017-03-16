#version 440 core
#extension GL_ARB_shader_draw_parameters : enable
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 tex;
layout (location = 2) in vec3 normal;

layout (std430, binding = 0) readonly buffer ModelTransforms
{
    mat4 modelMatrix[];
};

layout (std430, binding = 1) readonly buffer MvpTransforms
{
    mat4 mvpMatrix[];	
};

out vec2 texCoord;
out vec3 normal_World;

void main()
{
	gl_Position = mvpMatrix[gl_DrawIDARB] * vec4(position, 1.0);
	texCoord = tex;
	normal_World = normalize(mat3(modelMatrix[gl_DrawIDARB]) * normal);
}