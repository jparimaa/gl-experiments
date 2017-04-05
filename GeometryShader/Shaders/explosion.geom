#version 450 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

layout (location = 2) uniform float time;

in vec2 texVS[];
in vec3 normalWorldVS[];

out vec2 texGS;
out vec3 normalWorldGS;

vec3 getDirection() 
{
   vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
   vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
   return normalize(cross(a, b));
}  

vec4 explode(vec4 position, vec3 dir) 
{
    float magnitude = 1.0f;
    vec3 direction = dir * ((sin(time) + 1.0f) / 2.0f) * magnitude; 
    return position + vec4(direction, 0.0f);
}

void main() 
{
	vec3 dir = getDirection();
	
	for (int i = 0; i < 3; ++i) {
		gl_Position = explode(gl_in[i].gl_Position, dir);
		texGS = texVS[i];
		normalWorldGS = normalWorldVS[i];
		EmitVertex();
	}    
    EndPrimitive();
}