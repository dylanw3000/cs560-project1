#version 430 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 norm;
layout(location = 2) in vec2 tex;
layout(location = 3) in ivec4 boneIds; 
layout(location = 4) in vec4 weights;

uniform mat4 model;
uniform mat4 camMatrix;

out vec2 TexCoords;
out vec3 FragPos;
out vec3 Normal;

void main()
{
	TexCoords = tex;
	FragPos = vec3(model * vec4(pos, 1.0));
    Normal = mat3(transpose(inverse(model))) * norm;  
	gl_Position = camMatrix * model * vec4(pos, 1.0f);
}