#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

out vec3 ourColor;
out vec2 TexCoord;

uniform float u_time;
uniform vec2 u_resolution;

bool isinrec(vec2 bl, vec2 tr, vec2 st)
{
    vec2 s1 = step(bl, st);
    vec2 s2 = step(tr, 1.0 - st); // 1.0 -st
    return s1.x * s1.y * s2.x * s2.y == 0.0 ? false : true;
}

void main()
{
	
	vec4 newPos = vec4(1.0);

	if(gl_Position.y * gl_Position.x < 0.0)
	{
		newPos.x = 0.002;
	}
	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
	ourColor = aColor;
	if(gl_Position.y * gl_Position.x > 0.0)
	{
		newPos.x = 0.002;
	}

	TexCoord = vec2(aTexCoord.x, aTexCoord.y) * newPos.xy;
}