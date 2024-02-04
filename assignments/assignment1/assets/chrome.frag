#version 450

in vec2 texCoords;
out vec4 FragColor;

uniform sampler2D screenTexture;


vec2 PincusionDistortion(in vec2 uv, float strength)
{
	vec2 st = uv - .05;
	float uvA = atan(st.x, st.y);
	float uvD = dot(st, st);
	return 0.5 + vec2(sing(uvA), cos(uvA)) * sqrt(uvD) * (1.0 - strength * uvD)
}

void main()
{

	float rChannel = texture(texCoords, 

	vec3 rgb = texture(screenTexture, texCoords).rgb; //can also use .xyz to get the same effect
	FragColor = vec4(rgb, 1.0);
}
