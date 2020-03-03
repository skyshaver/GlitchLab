#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

// texture sampler
uniform sampler2D texture1;

uniform float colorValue;

void main()
{
	// FragColor = texture(texture1, TexCoord);
	FragColor = texture(texture1, TexCoord) * vec4( ourColor.x,
													ourColor.y,
													ourColor.z,
													 1.0);
}
