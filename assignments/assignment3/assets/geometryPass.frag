#version 450

layout(location = 0) out vec3 glWorldPosition;
layout(location = 1) out vec3 glWorldNormal;
layout(location = 2) out vec3 glAlbeto;

in vec2 UV;
in vec3 WorldPos;
in vec3 WorldNormal;


uniform sampler2D _MainText;

void main()
{
	glWorldPosition = WorldPos;

	glWorldNormal = normalize(WorldNormal);

	glAlbeto = texture(_MainText, UV).rgb; //glWorldNormal * 0.5;

}