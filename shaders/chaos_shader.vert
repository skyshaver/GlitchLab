#version 330 core
layout (location = 0) in vec3 aPos; // combine texcoords which are 2d with pos before this will work
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

out vec3 ourColor;
out vec2 TexCoord;

uniform float u_time;

void main()
{
	gl_Position = vec4(aPos.xy, 0.0, 1.0);
	vec2 texture = aPos.zw;
	//ourColor = aColor;
	//TexCoord = vec2(aTexCoord.x, aTexCoord.y);
	//
	float strength = 0.3;
    vec2 pos = vec2(texture.x + sin(u_time) * strength, texture.y + cos(u_time) * strength);        
    TexCoords = pos;
}