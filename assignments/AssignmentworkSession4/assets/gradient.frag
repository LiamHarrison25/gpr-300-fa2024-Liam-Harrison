#version 450

out vec4 FragColor;

struct Transition
{
	vec3 color;
	float cutoff;
	sampler2D gradient;
};
uniform Transition transition;

in vec2 texCoords;

void main()
{
	
	float N = texture(transition.gradient ,texCoords).r;

	vec3 color = texture(transition.gradient ,texCoords).rgb;

	if(N >= transition.cutoff)
	{
		discard; 
	}

	FragColor = vec4(color, 1.0);
}
