#version 450

layout(location = 0) in vec2 vPos; //quad positions
layout(location = 1) in vec2 vTexCoord;

out vec2 texCoords;

void main()
{
	texCoords = vTexCoord;
	gl_Position = vec4(vPos, 0.0, 1.0);
}
