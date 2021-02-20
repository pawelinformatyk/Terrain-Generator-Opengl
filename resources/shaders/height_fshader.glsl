#version 330

out vec4 FragColor;

in vec3 FragPos;  

uniform float maximum;

void main()
{
    float color=int(10*FragPos.y/maximum)/10.;
    vec3 result =  vec3(color,1-color,0);
    
    FragColor =  vec4(result, 1.0);
}