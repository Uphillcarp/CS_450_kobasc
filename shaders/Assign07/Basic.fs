#version 430 core
// Change to 410 for macOS

layout(location=0) out vec4 out_color;
 
in vec4 vertexColor; // Now interpolated across face
in vec4 interPos;
in vec3 interNormal;

struct PointLight {  
	vec4 pos; 
	vec4 color; 
};

uniform float metallic; 
uniform float roughness;
uniform PointLight light;

const float PI = 3.14159265359;

vec3 getFresnelAtAngleZero(vec3 albedo, float metallic){
	vec3 F0 = vec3(0.04);

	F0 = mix(F0, albedo, metallic);

	return F0;
}

vec3 getFresnel(vec3 F0, vec3 L, vec3 H){
	float cosAngle = max(0,dot(L,H));

	vec3 fresnel = F0+(1-F0)*(pow((1-max(0,cosAngle)),5));

	return fresnel;
}

 float getNDF(vec3 H, vec3 N, float roughness){
	float a2 = pow(pow(roughness, 2), 2);
	float NH = pow(dot(N, H), 2);
	float dminator = PI * pow(((NH)*(a2 - 1) + 1), 2);

	float ndf = a2 / dminator;

	return ndf;
 }

 float getSchlickGeo(vec3 B, vec3 N, float roughness){
	float k = pow(roughness + 1, 2)/8;
	float schlickGeo =  dot(N, B) / (dot(N, B)*(1 - k) + k);

	return schlickGeo; 
 }

 float getGF(vec3 L, vec3 V, vec3 N, float roughness){
	float GL = getSchlickGeo(L, N, roughness);
	float GV = getSchlickGeo(V, N, roughness);

	return GL * GV;
 }

void main()
{	
	vec3 N = normalize(interNormal);
	vec3 L =  normalize(vec3(light.pos) - vec3(interPos));
	float diff = max(0, dot(L,N));
	float shininess = 10.0;
	
	vec3 diffColor = diff*vec3(vertexColor)*vec3(light.color);

	vec3 F0 = getFresnelAtAngleZero(vec3(vertexColor), metallic);
	vec3 V = normalize(vec3(-interPos));
	vec3 H = normalize(L + V);

	vec3 F = getFresnel(F0, L, H); 

	vec3 kS = F;
	vec3 kD = 1.0 - kS;

	kD *= (1.0 - metallic);

	kD *= vec3(vertexColor);
	kD /= PI;

	float specular = diff * pow(max(0, dot(N,H)), shininess);
	vec3 specularColor = specular*vec3(light.color);

	float NDF = getNDF(H, N, roughness);
	float G = getGF(L, V, N, roughness);

	kS *= NDF*G;
	kS /= (4.0 * max(0, dot(N,L)) * max(0, dot(N,V))) + 0.0001;

	vec3 finalColor = (kD + kS)*vec3(light.color)*max(0, dot(N,L));

	// Just output interpolated color
	out_color = vec4(finalColor, 1.0);
}
