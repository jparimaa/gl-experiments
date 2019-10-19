#version 450 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 tex;

layout (location = 0) uniform mat4 MVP;
layout (location = 1) uniform mat4 projectorMVP;

out vec2 texCoord;
out vec2 projectionUvPosition;

void main()
{
	gl_Position = MVP * vec4(position, 1.0);
	vec4 projPos = projectorMVP * vec4(position, 1.0);
	projectionUvPosition = (projPos.xyz / projPos.w).xy;
	texCoord = tex;
}