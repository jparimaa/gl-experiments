#version 450 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 tex;
layout (location = 2) in vec3 normal;

layout (location = 1) uniform mat4 MV;
layout (location = 2) uniform mat4 M;

out vec2 texVS;
out vec3 normalWorldVS;

void main()
{
	gl_Position = MV * vec4(position, 1.0);
	texVS = tex;
	normalWorldVS = mat3(M) * normal;
}