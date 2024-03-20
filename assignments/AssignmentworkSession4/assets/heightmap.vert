#version 450

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vTexCoord;

uniform mat4 _Model;
uniform mat4 _ViewProjection;
uniform sampler2D heightmap;
uniform float maxHeight;
//uniform vec3 newColor;

//out vec3 islandColor;
out float scalarOut;
out vec2 TexCoord;


void main()
{
	vec3 position = vPos;

	float scalar = texture(heightmap, vTexCoord * 1.0f).r;

	//vec3 islandColor = newColor;

	scalarOut = scalar;
	
	position.y = scalar * maxHeight;
	
	TexCoord = vTexCoord;
	gl_Position = _ViewProjection * _Model * vec4(position,1.0);
}
