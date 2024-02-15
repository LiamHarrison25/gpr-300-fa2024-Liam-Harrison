#version 450

out vec4 FragColor;
in vec2 TexCoord;

uniform vec3 lightPos;
uniform vec3 ambientLight;
uniform vec3 cameraPos;
uniform vec3 worldNormal;
uniform vec3 lightColor;

struct pbrMaterial
{
	sampler2D albeto;
	sampler2D metalic;
	sampler2D occlusion;
	sampler2D specular;
};

//cache
vec3 col;
float mtl;
float ao;
float spc;
float rgh;

const float epsilon = 0.0000005;
float PI = 3.14159;

float nDotH;
float nDotV; 
float nDotL;
float vDotH;
float vDotN;
float lDotN;

vec3 N = normalize(worldNormal);
vec3 V = normalize(cameraPos);
vec3 L = normalize(lightPos);
vec3 H = normalize(V + L);

//ggx
float D(float alpha)
{
	float numerator = pow(alpha, 2.0);
	float denominator = PI * pow(pow(nDotH, 2.0) * (pow(alpha, 2.0) - 1) + 1, 2.0);

	return numerator / denominator;
}

//schlicc beckman
float G1(float alpha, float x)
{

	float k = alpha / 2;
	float denominator = x * (1.0 - k) + k;
	return x / denominator; 

}

//smith
float G(float alpha)
{
	return G1(alpha, lDotN) * G1(alpha,vDotH); 
}


//fresnal schlicc
vec3 F(vec3 F0)
{
	return F0 + (1 - F0) * pow(1 - vDotH, 5);
}

vec3 pbr()
{
	vec3 F0 = vec3(0.4);
	//F0 = col;
	//F0 = vec3(mtl);

	vec3 ks = F(F0);
	vec3 Kd = vec3((1.0) - ks) * (1.0 - mtl);

	float alpha = pow(ao, 2.0);

	vec3 lambert = col / PI;

	vec3 cookTorranceNumerator = D(alpha) * G(alpha) * ks;
	float cookTorranceDenominator = max(4.0 * vDotN + vDotN, epsilon);
	vec3 cookTorrance = cookTorranceNumerator / cookTorranceDenominator;

	vec3 BDRF = (Kd * lambert) + cookTorrance;

	return BDRF;
}



void main()
{
	//presample
	col = texture(pbrMaterial.albeto, TexCoord).rgb;
	//mtl = texture(

	//precomputing
	nDotH = max( dot(N, H), 0.0);
	nDotV = max( dot(N, V), 0.0); 
	nDotL = max( dot(N, L), 0.0);
	vDotH = max( dot(V, H), 0.0);
	vDotN = max( dot(V, N), 0.0);
	lDotN = max( dot(L, N), 0.0);

	//stylization:
	vec3 reflectionDir = reflect(-L ,N);
	float specAmount = pow(max(dot(V, reflectionDir), 0.0), 32);
	vec3 specular = spec * specAmount * lightColor;

	vec3 finalColor = (ambientLight * col * ab) + PBR();

	FragColor = vec4(finalColor, 1);
}