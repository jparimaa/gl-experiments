#version 450 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 tex;
layout (location = 2) in vec3 normal;

layout(binding = 0) uniform transform
{
	mat4 M;
	mat4 MVP;
};

out vec2 texCoord;
out vec3 normal_World;

void main()
{	
	gl_Position = MVP * vec4(position, 1.0);
	texCoord = tex;
	normal_World = normalize(mat3(M) * normal);
}