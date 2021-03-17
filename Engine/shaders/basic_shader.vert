#version 330

layout(location = 0) in vec3 vPosition;

uniform mat4 vertexTransform;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	vec4 vPos = vec4( vPosition, 1.0f );
	gl_Position = projection*view*vertexTransform * vPos;
}