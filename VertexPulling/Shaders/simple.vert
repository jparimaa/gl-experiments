#version 450 core
layout (location = 0) uniform mat4 M;
layout (location = 1) uniform mat4 MVP;
layout (binding = 1) uniform samplerBuffer vnBuffer;
layout (binding = 2) uniform samplerBuffer uvBuffer;

out vec2 texCoord;
out vec3 normal_World;

void main()
{
	// Use texture buffers instead of vertex attributes
	vec3 position = texelFetch(vnBuffer, gl_VertexID * 2).xyz; 	
	vec3 normal = texelFetch(vnBuffer, gl_VertexID * 2 + 1).xyz;
	vec2 tex = texelFetch(uvBuffer, gl_VertexID).xy;
	
	gl_Position = MVP * vec4(position, 1.0);
	texCoord = tex;
	normal_World = normalize(mat3(M) * normal);
}