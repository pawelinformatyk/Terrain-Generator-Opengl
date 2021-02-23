#version 330

struct Material
{
    sampler2D diffuse;
    sampler2D specular;
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
uniform int terrain_size;
uniform int terrain_step;

void main()
{
    vec2 texture_cords=vec2(  
    (FragPos.x+(terrain_size-terrain_step)/2)/(terrain_size-terrain_step),
    (FragPos.z+(terrain_size-terrain_step)/2)/(terrain_size-terrain_step));

// ambient
    vec3 ambient = light.ambient * texture(material.diffuse,texture_cords).rgb;
  	
// diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * light.diffuse * texture(material.diffuse,texture_cords).rgb;

// specular
    vec3 viewDir = normalize(viewPos - FragPos);
    //phong
    //vec3 reflectDir = reflect(-lightDir, norm);
    //float spec = pow(max(dot(viewDir, reflectDir), 0.0), 8.0);
    //Blinn-Phong
    vec3 halfwayDir= normalize(lightDir+viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);
    vec3 specular = texture(material.specular,texture_cords).rgb* spec * light.specular;
   
    vec3 result = ( ambient + diffuse +specular) ;
    FragColor =vec4(result,1);
}