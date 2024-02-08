#version 450

in vec2 uv;
out vec4 FragColor;

uniform sampler2D debugImage;

void main()
{
	float depth = texture(debugImage, uv).r;
	FragColor = vec4(vec3(depth), 1.0);
}