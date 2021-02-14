#version 330

out vec4 FragColor;

in vec3 Normal;  
in vec3 FragPos;  

uniform vec3 lightPos; 
uniform vec3 lightColor;
uniform vec3 objectColor;
uniform vec3 viewPos;

uniform sampler2D tex;
uniform int terrain_size;
uniform int terrain_step;


void main()
{
// ambient
    float ambientStrength = 1;
    vec3 ambient = ambientStrength * lightColor;
  	
// diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

// specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;
    
    vec2 texture_cords=vec2(  1-(FragPos.x+terrain_size/2)/(terrain_size-terrain_step) ,(FragPos.z+terrain_size/2)/(terrain_size-terrain_step) );

    vec3 result = ( ambient + diffuse +specular)* objectColor ;
    FragColor =   texture( tex, texture_cords )*vec4(result,1);
} 