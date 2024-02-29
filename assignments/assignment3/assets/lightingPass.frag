#version 450

in vec2 texCoords;
out vec4 FragColor;

uniform layout (location = 0) sampler2D gPosition;
uniform layout(location = 1) sampler2D gNormal;
uniform layout (location = 2) sampler2D gAlbedo;

uniform vec3 _EyePos;
vec3 _LightDirection = vec3(0.0, -1.0, 0.0);
vec3 _LightColor = vec3(1.0);
vec3 _AmbientColor = vec3(0.3, 0.4, 0.46);

struct Material
{
	float Ka; //Ambient Coefficient
	float Kd; //Diffuse Coefficient
	float Ks; //Specular Coefficient
	float Shininess;
};
uniform Material _Material;


vec3 calculateLighting(vec3 position, vec3 normal, vec3 albedo)
{

	vec3 toLight = -_LightDirection;
	float diffuseFactor = max(dot(normal, toLight), 0.0);
	vec3 toEye = normalize(_EyePos - position);
	vec3 h = normalize(toLight + toEye);
	float specularFactor = pow(max(dot(normal, h), 0.0), _Material.Shininess);

	vec3 lighting = (_Material.Kd * diffuseFactor + _Material.Ks * specularFactor) * _LightColor;// = albedo;// * 0.1;
	lighting += _AmbientColor * _Material.Ka;

	//vec3 lightDir = normalize(_LightDirection - position);
	//vec3 diffuse = max(dot(normal, lightDir), 0.0) * albedo * _LightColor;

	//lighting += diffuse;


	return lighting * albedo; 
}

void main()
{
	vec3 FragPos = texture(gPosition, texCoords).rgb;
	vec3 Normal = texture(gNormal, texCoords).rgb;
	vec3 Albedo = texture(gAlbedo, texCoords).rgb;

	vec3 lighting = calculateLighting(FragPos, Normal, Albedo);

	FragColor = vec4(Albedo * lighting, 1.0);
}