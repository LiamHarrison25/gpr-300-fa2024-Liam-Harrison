#version 450


//new material and lighting uniforms:

uniform layout(binding = 0) sampler2D gPositions;
uniform layout(binding = 1) sampler2D gNormals;
uniform layout(binding = 2) sampler2D gAlbedo;

//---------------------


out vec4 FragColor;
in vec2 texCoords;

uniform sampler2D _MainTex;
uniform vec3 _EyePos;
uniform vec3 _LightDirection = vec3(0.0, -1.0, 0.0);
uniform vec3 _LightColor = vec3(1.0);
uniform vec3 _AmbientColor = vec3(0.3, 0.4, 0.46);

struct Material
{
	float Ka; //Ambient Coefficient
	float Kd; //Diffuse Coefficient
	float Ks; //Specular Coefficient
	float Shininess;
};
uniform Material _Material;

struct PointLight
{
	vec3 position;
	float radius;
	vec4 color;
};
#define MAX_POINT_LIGHTS 64
uniform PointLight _PointLights[MAX_POINT_LIGHTS];


float attenuateLinear(float distance, float radius)
{
	return clamp((radius - distance) / radius, 0.0, 1.0);
}

vec3 CalculateDirectionalLight(vec3 normal, vec3 toEye)
{
	//vec3 normal = normalize(fs_in.WorldNormal);
	vec3 toLight = -_LightDirection;
	float diffuseFactor = max(dot(normal, toLight), 0.0);

	//vec3 toEye = normalize(_EyePos - fs_in.WorldPos);
	vec3 h = normalize(toLight + toEye);
	float specularFactor = pow(max(dot(normal, h), 0.0), _Material.Shininess);

	vec3 lightColor = (_Material.Kd * diffuseFactor + _Material.Ks * specularFactor) * _LightColor;

	//vec3 diffuseColor = _LightColor * diffuseFactor;
	lightColor += _AmbientColor * _Material.Ka;
	//vec3 objectColor = texture(_MainTex, fs_in.TexCoord).rgb;
	return lightColor;
}

vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 pos)
{
	vec3 diff = light.position - pos;
	vec3 toLight = normalize(diff);

	float diffuseFactor = max(dot(normal, toLight), 0.0);

	//vec3 toEye = normalize(_EyePos - fs_in.WorldPos);
	vec3 h = normalize(toLight + light.position);
	float specularFactor = pow(max(dot(normal, h), 0.0), _Material.Shininess);

	vec3 lightColor = (diffuseFactor + specularFactor) * light.color.rgb;

	float d = length(diff);
	lightColor*=attenuateLinear(d, light.radius);

	return lightColor;
}

void main()
{
	vec3 normal = texture(gNormals, texCoords).xyz;
	vec3 pos = texture(gPositions, texCoords).xyz;
	vec3 albedo = texture(gAlbedo, texCoords).xyz;

	vec3 totalLight = vec3(0);

	totalLight += CalculateDirectionalLight(normal, pos);

	int i;
	for(i = 0; i < MAX_POINT_LIGHTS; i++)
	{
		totalLight += CalculatePointLight(_PointLights[i], normal, pos);
	}
	
	FragColor = vec4(albedo * totalLight, 1.0);
}
