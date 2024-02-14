#version 450

layout(location = 0) in vec3 vPos;

uniform mat4 _Model;
uniform mat4 lightSpaceMatrix;

void main()
{
	gl_Position = lightSpaceMatrix * _Model * vec4(vPos,1);
}
