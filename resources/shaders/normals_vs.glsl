#version 330 

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out VS_OUT 
{
    vec3 normal;
} vs_out;

uniform mat4 MVP;

void main()
{
    vs_out.normal=normalize(aNormal);
    gl_Position = MVP*vec4(aPos, 1.0f);
}