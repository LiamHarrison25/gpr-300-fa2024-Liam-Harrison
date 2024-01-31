#version 450

in vec2 texCoords;
out vec4 FragColor;

uniform sampler2D screenTexture;

void main()
{
	//vec2 UV = texCoords / textureSize.xy;
	
	vec2 texelSize = 1.0 / textureSize(screenTexture, 0).xy;
	vec3 totalColor = vec3(0);
	
	//vec3 rgb = texture(screenTexture, texCoords).rgb; //can also use .xyz to get the same effect

	int x, y;
	for (y = -2; y <=2; y++)
	{
		for(x = -2; x <=2; x++)
		{
			vec2 offset = vec2(x,y) * texelSize;
			totalColor += texture(screenTexture, texCoords + offset).rgb;
		}
	}

	totalColor/=(5*5);
	FragColor = vec4(totalColor, 1.0);
}