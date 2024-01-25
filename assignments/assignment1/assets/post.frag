#version 450

in vec2 texCoords;
out vec4 FragColor;

uniform sampler2D screenTexture;

void main()
{
	vec3 rgb = texture(screenTexture, texCoords).rgb; //can also use .xyz to get the same effect
	FragColor = vec4(rgb, 1.0);
}
