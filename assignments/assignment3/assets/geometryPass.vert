#version 450
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 WorldPos;
out vec2 UV;
out vec3 WorldNormal;

uniform mat4 _Model;
uniform mat4 _ViewProjection;

void main()
{
	WorldPos = vec3(_Model * vec4(aPos,1.0));
	WorldNormal = transpose(inverse(mat3(_Model))) * aNormal;
	UV = aTexCoords;

	gl_Position = _ViewProjection * _Model * vec4(aPos,1.0);
}