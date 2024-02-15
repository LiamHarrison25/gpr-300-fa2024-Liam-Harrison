#version 450

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vTexCoord;

uniform mat4 _Model;
uniform mat4 _ViewProjection;
uniform float waveStrength;
uniform float waveScale;
uniform float _time;

out vec2 TexCoord;


float calculateSurface(float _x, float _z)
{
	float y, x = 0.0f;
	y += sin(_x  * 0.75 / waveScale + _time) + sin(_x  * 0.25 / waveScale + _time) + sin(_x  * 0.55 / waveStrength + _time);
	y += sin(_z  * 0.5 / waveScale + _time) + sin(_z  * 0.35 / waveScale + _time) + sin(_z  * 0.58 / waveStrength + _time);
	return y;
}

void main()
{
	vec3 position = vPos;
	position.y = calculateSurface(position.x, position.z);

	TexCoord = vTexCoord;
	gl_Position = _ViewProjection * _Model * vec4(position,1.0);
}
