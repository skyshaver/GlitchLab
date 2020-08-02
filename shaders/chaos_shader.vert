#version 330 core
layout (location = 0) in vec4 vertex; // combine texcoords which are 2d with pos before this will work

out vec2 TexCoords;

uniform float u_time;

void main()
{
	gl_Position = vec4 (vertex.xy, 0.0, 1.0);
	vec2 texture = vertex.zw;
	
	float strength = 0.3;
    vec2 pos = vec2(texture.x + sin(u_time) * strength, texture.y + cos(u_time) * strength);        
    TexCoords = pos;
}