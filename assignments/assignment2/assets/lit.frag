#version 450

out vec4 FragColor;
in Surface
{
	vec4 LightSpacePos; 
	vec3 WorldPos;
	vec3 WorldNormal;
	vec2 TexCoord;
}fs_in;

uniform sampler2D _shadowMap;

uniform sampler2D _MainTex;
uniform vec3 _EyePos;
uniform vec3 _LightDirection = vec3(0.0, -1.0, 0.0);
uniform vec3 _LightColor = vec3(1.0);
uniform vec3 _AmbientColor = vec3(0.3, 0.4, 0.46);
uniform vec3 _lightPos;

struct Material
{
	float Ka; //Ambient Coefficient
	float Kd; //Diffuse Coefficient
	float Ks; //Specular Coefficient
	float Shininess;
};
uniform Material _Material;

float calcShadow(sampler2D shadowMap, vec4 lightSpacePos)
{

	vec3 sampleCoord = lightSpacePos.xyz / lightSpacePos.w; //performs perspective divide

	sampleCoord = sampleCoord * 05 + 0.5; //Transforms to the [0,1] range

	float currentDepth = sampleCoord.z;

	float closestDepth  = texture(shadowMap, sampleCoord.xy).r; //gets the closest depth value from lights perspective (using [0,1] range)

	vec3 normal = normalize(fs_in.WorldNormal);
    vec3 lightDir = normalize(_lightPos - fs_in.WorldPos);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);

	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(_shadowMap, 0);
	int x, y; 
//	for(x = -1; x <= 1; ++x)
//	{
//		for(x = -1; y <= 1; ++y)
//		{
//			float pcfDepth = texture(_shadowMap, sampleCoord.xy + vec2(x, y) * texelSize).r;
//			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
//		}
//	}

	return step(closestDepth , currentDepth);
}

void main()
{
	float shadow = calcShadow(_shadowMap, fs_in.LightSpacePos);
	vec3 normal = normalize(fs_in.WorldNormal);
	vec3 toLight = -_LightDirection;
	float diffuseFactor = max(dot(normal, toLight), 0.0);

	vec3 toEye = normalize(_EyePos - fs_in.WorldPos);
	vec3 h = normalize(toLight + toEye);
	float specularFactor = pow(max(dot(normal, h), 0.0), _Material.Shininess);

	vec3 lightColor = (_Material.Kd * diffuseFactor + _Material.Ks * specularFactor) * _LightColor;

	vec3 diffuseColor = _LightColor * diffuseFactor;
	//lightColor += (_AmbientColor * _Material.Ka);
	vec3 objectColor = texture(_MainTex, fs_in.TexCoord).rgb;

	vec3 lighting = (_AmbientColor + (1.0 - shadow) * (diffuseColor + specularFactor) * objectColor);

	FragColor = vec4(lighting, 1.0);

}
