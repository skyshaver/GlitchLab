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
    vec2 s1 = step(bl,st);
    vec2 s2 = step(tr,1.0-st);
    return s1.x * s1.y * s2.x * s2.y == 0.0 ? false : true;
}

void main()
{
	
	gl_Position = vec4(aPos, 1.0);
	ourColor = aColor;

	vec2 st = aTexCoord.xy / u_resolution.xy;

	// vec space normalization
	vec3 ndc = gl_Position.xyz / gl_Position.w; //perspective divide/normalize
	vec2 viewportCoord = ndc.xy * 0.5 + 0.5; //ndc is -1 to 1 in GL. scale for 0 to 1
	// vec2 st = viewportCoord / u_resolution;

	// var for texcoord
	vec2 ourTexCoord = vec2(aTexCoord.x, aTexCoord.y);

	// create rect
	vec2 bl = vec2(0.0, 0.0);
    vec2 tr = vec2(1., 1.);

    if(isinrec(bl, tr, st))
        ourTexCoord = vec2(abs(sin(u_time)), aTexCoord.y);

	TexCoord = vec2(ourTexCoord.x, ourTexCoord.y);
}