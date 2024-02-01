#version 450

in vec3 uv;
out vec4 FragColor;

void main()
{
	FragColor = vec4(uv, 1.0);
}