#version 450

out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D _MainTex;
uniform float scale;
uniform float _time;


void main()
{

	vec2 uv = TexCoord * scale + vec2(_time);
	uv.x += sin(uv.y + _time) + sin(uv.y * 0.2 + _time * 0.03);
	uv.y += sin(uv.x + _time) + sin(uv.x * 0.02 + _time * 0.01);
	vec3 waterColor = vec3(0.0f, 0.31f, 0.85f);

	vec4 sample1 = texture(_MainTex, uv * 1.0f).rgba;
	vec4 sample2 = texture(_MainTex, uv * 1.0f).rgba * 1.0f * vec4(0.2);
	FragColor = vec4(waterColor + vec3(sample1 * .09 - sample2 * .02), 1.0);
}
