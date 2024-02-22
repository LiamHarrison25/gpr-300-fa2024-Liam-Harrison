#version 450

out vec4 FragColor;
in vec2 TexCoord;
in float scalarOut;
in vec3 islandColor;

uniform sampler2D _MainTex;
uniform float scale;
uniform float _time;
uniform vec3 newColor;


void main()
{

	vec2 uv = TexCoord * scale; //+ vec2(_time);
	//uv.x += sin(uv.y + _time) + sin(uv.y * 0.2 + _time * 0.03);
	//uv.y += sin(uv.x + _time) + sin(uv.x * 0.02 + _time * 0.01);
	//vec3 islandColor = vec3(0.0f, 0.31f, 0.85f);

	vec3 color = newColor;


	color.y = scalarOut;
	//vec3 objectColor = texture(_MainTex, TexCoord).rgb;
	//objectColor.y = scalarOut;
	
	FragColor = vec4(color, 1.0);
}
