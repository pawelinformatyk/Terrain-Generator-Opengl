#version 330

struct Material
{
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light
{
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

out vec4 FragColor;

in vec3 Normal;  
in vec3 FragPos;  

uniform Light light;
uniform Material material;
uniform vec3 viewPos;


void main()
{
// ambient
    vec3 ambient = material.diffuse * light.ambient;
  	

// diffuse 
    vec3 norm = Normal;
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * light.diffuse*material.diffuse;


// specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = material.specular * spec * light.specular;
    

    vec3 result =ambient+diffuse+specular;
    FragColor =  vec4(result, 1.0);
} 