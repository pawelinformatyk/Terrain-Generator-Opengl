#version 330 

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 normal;

uniform mat4 VM;
uniform mat4 projection;

void main()
{
    gl_Position = projection *VM * vec4(aPos, 1.0);
    mat3 normalMatrix = mat3(transpose(inverse(VM)));
    normal = normalize(vec3(projection* vec4(normalMatrix * aNormal, 0.0)));
}