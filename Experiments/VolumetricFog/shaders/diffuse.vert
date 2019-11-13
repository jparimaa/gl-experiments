#version 450 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 tex;
layout (location = 2) in vec3 normal;

layout (location = 0) uniform mat4 MVP;
layout (location = 1) uniform mat4 M;

out vec2 texCoord;
out vec3 worldPos;
out vec3 worldNormal;

void main()
{
	gl_Position = MVP * vec4(position, 1.0);
	texCoord = tex;
	worldPos = vec3(M * vec4(position, 1.0));
	worldNormal = normalize(vec3(M * vec4(normal, 0.0)));
}