#version 330
uniform vec3 objectColor;	

out vec4 outcolor;

void main()
{
	outcolor = vec4(objectColor,1);
}