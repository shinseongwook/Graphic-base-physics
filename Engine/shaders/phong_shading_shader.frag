#version 330 core

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
}; 

struct Light {
	int is_mix_all;
	int is_position;
	int is_normal;
	int is_albedo;
	int is_specular;
	
    vec3 position;
	vec3 direction;
    float cutOff;
    float outerCutOff;
	float falloff;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
#define NR_POINT_LIGHTS 8

uniform Light Lights[NR_POINT_LIGHTS];
uniform Material material;
uniform vec3 GlobalIemissive;
uniform vec3 GlobalAmbientColor;
uniform vec3 FogColor;
uniform vec3 Ka;
uniform float constant;
uniform float linear;
uniform float quadratic;

uniform float near_distance;
uniform float far_distance;

uniform vec3 viewPos;

uniform sampler2D fbo_position;
uniform sampler2D fbo_normal;
uniform sampler2D fbo_albedospec;

in vec2 TexCoords;
out vec4 FragColor;


void main()
{  
    vec3 FragPos = texture(fbo_position, TexCoords).rgb;
    vec3 Normal = texture(fbo_normal, TexCoords).rgb;
    vec3 Diffuse = texture(fbo_albedospec, TexCoords).rgb;
    float Specular = texture(fbo_albedospec, TexCoords).a;
    
    vec3 lighting  = Diffuse * 0.05; // hard-coded ambient component
    vec3 viewDir  = normalize(viewPos - FragPos);
	
	if(Lights[0].is_mix_all==0)
	{
		if(Lights[0].is_position==1)
		{
			FragColor=vec4(FragPos,1.0);
		}
		if(Lights[0].is_normal==1)
		{
			FragColor=vec4(Normal,1.0);
		}
		if(Lights[0].is_albedo==1)
		{
			FragColor=vec4(Diffuse,1.0);
		}
		return;
	}
	
	for(int i = 0; i < NR_POINT_LIGHTS; i++)
    {
        // diffuse
        vec3 lightDir = normalize(Lights[i].position - FragPos);
        vec3 diffuses = max(dot(Normal, lightDir), 0.0) * Diffuse * Lights[i].diffuse;
        // specular
        vec3 halfwayDir = normalize(lightDir + viewDir);  
        float spec = pow(max(dot(Normal, halfwayDir), 0.0), 16.0);
        vec3 specular = Lights[i].diffuse * spec * Specular;
        // attenuation
        float distance = length(Lights[i].position - FragPos);
        float attenuation = 1.0 / (1.0 + linear * distance + quadratic * distance * distance);
        diffuses *= attenuation;
        specular *= attenuation;
        lighting += diffuses + specular;        
    }
	FragColor=vec4(lighting,1.0);
}