#version 330 core

out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

// texture sampler
uniform sampler2D texture1;

uniform float u_time;
uniform vec2 u_resolution;


void main()
{
	// rgb change over time
	// FragColor = texture(texture1, TexCoord) * vec4(abs(sin(u_time / 3)),
	// 											   abs(sin(u_time / 2)),
	// 											   abs(sin(u_time / 10)),
	// 											   1.000);
	
	// rainbow spread over texture
	// FragColor = texture(texture1, TexCoord) * vec4(ourColor, 1.0);
	

	// another way to spread rainbow over texture
	vec2 st = gl_FragCoord.xy / u_resolution;
	FragColor = texture(texture1, TexCoord) * vec4(st.x, st.y, 0.0, 1.0);
	
}
