#version 330 core
layout (location = 0) out vec3 g_position;
layout (location = 1) out vec3 g_normal;
layout (location = 2) out vec4 g_albedospec;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
}; 

uniform Material material;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

//out vec4 FragColor;

void main()
{  
    g_position=FragPos;
	g_normal=normalize(Normal);
	g_albedospec.rgb=texture(material.diffuse,TexCoords).rgb;
	g_albedospec.a=texture(material.specular,TexCoords).r;
	
	//FragColor=vec4(TexCoords.x,TexCoords.y,0,1);
}