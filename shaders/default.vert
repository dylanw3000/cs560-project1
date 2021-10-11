#version 430 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 norm;
layout(location = 2) in vec2 tex;
layout(location = 3) in ivec4 boneIds; 
layout(location = 4) in vec4 weights;

/*
uniform mat4 projection;
uniform mat4 view;
*/
uniform mat4 camMatrix;
uniform mat4 model;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBonesMatrices[MAX_BONES];

out vec2 TexCoords;
out vec3 FragPos;
out vec3 Normal;

void main()
{
    vec4 totalPosition = vec4(0.0f);
    vec3 localNormal = norm;
    for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++)
    {
        if(boneIds[i] == -1) 
            continue;
        if(boneIds[i] >=MAX_BONES) 
        {
            totalPosition = vec4(pos,1.0f);
            break;
        }
        vec4 localPosition = finalBonesMatrices[boneIds[i]] * vec4(pos,1.0f);
        totalPosition += localPosition * weights[i];
        // localNormal *= mat3(finalBonesMatrices[boneIds[i]]);
		localNormal = mat3(transpose(inverse(model))) * mat3(finalBonesMatrices[boneIds[i]]) * norm;
		Normal = localNormal;
   }
	
    FragPos = vec3(model * vec4(pos, 1.0));
    // Normal = mat3(transpose(inverse(model))) * localNormal;  

    gl_Position =  camMatrix * model * totalPosition;
	TexCoords = tex;
}
