#version 450 core

// Draw a triangle which covers the whole screen
const vec2 vertices[3] = vec2[](vec2(-1, -1), vec2(3, -1), vec2(-1, 3));
const vec2 uvs[3] = vec2[](vec2(0, 0), vec2(2, 0), vec2(0, 2));

out vec2 uv;

void main() 
{
	uv = uvs[gl_VertexID];
	gl_Position = vec4(vertices[gl_VertexID], 0.0, 1.0);
}