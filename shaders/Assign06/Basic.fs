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

uniform PointLight light;

void main()
{	
	vec3 N = normalize(interNormal);
	vec3 L =  normalize(vec3(light.pos) - vec3(interPos));
	float diff = max(0, dot(L,N));
	float shininess = 10.0;
	
	vec3 diffColor = diff*vec3(vertexColor)*vec3(light.color);
	vec3 V = normalize(vec3(-interPos));
	vec3 H = normalize(L + V); 

	float specular = diff * pow(max(0, dot(N,H)), shininess);
	vec3 specularColor = specular*vec3(light.color);

	// Just output interpolated color
	out_color = vec4(diffColor + specularColor, 1.0);
}
