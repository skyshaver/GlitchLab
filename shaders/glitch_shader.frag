#version 330 core

out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

uniform vec2 u_resolution;
uniform vec2 u_mousePos;
uniform float u_time;

// texture sampler
uniform sampler2D texture1;

bool isinrec(vec2 bl, vec2 tr, vec2 st)
{
    vec2 s1 = step(bl, st);
    vec2 s2 = step(tr, 1.0 - st); // 1.0 -st
    return s1.x * s1.y * s2.x * s2.y == 0.0 ? false : true;
}


void main()
{
	vec2 st = gl_FragCoord.xy/u_resolution;

	 
	 float pct = 0.0;

    // a. The DISTANCE from the pixel to the center
    // * 2.0 - 0.5 scales it down to fit in the window
    pct = distance(st * 2.0 - 0.5, vec2(0.5));

    vec3 color = {1.0, 1.0, 1.0};

    // vec2 bl = vec2(0.5, 0.1);
    // vec2 tr = vec2(0.1, 0.1);

    // if(isinrec(bl, tr, st))
    //     color = vec3(pct,abs(sin(u_time * 0.1)), 1);

    // vec2 bl_1 = vec2(0.1, 0.1);
    // vec2 tr_1 = vec2(0.5, 0.1);

    // if(isinrec(bl_1, tr_1, st))
    //     color = vec3(ourColor.r, ourColor.g * abs(sin(u_time * 0.1)), ourColor.b);
    

	FragColor = texture(texture1, TexCoord) * vec4(color, 1.0);
	
}