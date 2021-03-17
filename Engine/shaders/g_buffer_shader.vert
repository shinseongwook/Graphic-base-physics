#version 330 core
layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexCoords;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

uniform mat4 vertexTransform;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    FragPos = vec3(vertexTransform * vec4(vPosition, 1.0));
    Normal = mat3(transpose(inverse(vertexTransform)))*vNormal;
    TexCoords = vTexCoords;
    
    gl_Position = projection * view * vec4(FragPos, 1.0);
}