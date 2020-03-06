#version 330 core

in vec2 TexCoord;

out vec4 FragColor;

uniform vec2 u_resolution;
uniform vec2 u_mousePos;
uniform float u_time;

// texture sampler
uniform sampler2D texture1;

void main(){
    vec2 st = gl_FragCoord.xy/u_resolution.xy;
    vec3 color = vec3(0.0);
 

    // 1.2.
	float proportion = 0.1;
    vec2 bl = smoothstep(vec2(0.0), vec2(proportion),st);
    vec2 tr = smoothstep(vec2(0.0), vec2(proportion),1.0-st);

    float pct = bl.x * bl.y * tr.x * tr.y;
    color = vec3(pct);

    FragColor = texture(texture1, TexCoord) * vec4(color,1.0);
}