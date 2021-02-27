#version 330

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

uniform vec3 viewPos;
uniform Light light;
uniform float maximum;
uniform float minimum;

void main()
{
    vec3 mat_specular=vec3(0.1);
    float shininess=8;
    vec3 color;

    if(FragPos.y-minimum<=(maximum-minimum)/4)
    {
        color = mix(vec3(0,0,0.2),vec3(0,0,1), (FragPos.y-minimum)/( ( maximum-minimum)/4 ));
        mat_specular=vec3(0.255);
        shininess=1024;
    }
    else if(FragPos.y-minimum<=(maximum-minimum)/2)
    {
        color=vec3(0,0.5,0);
    }
    else  if(FragPos.y-minimum<=3*(maximum-minimum)/4)
    {
        color=vec3(0.5);
    }
    else
    {
        color =vec3(0.9);
    }

// ambient
    vec3 ambient = color*light.ambient;

// diffuse 
    vec3 norm = Normal;
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * light.diffuse*color;

// specular
    vec3 viewDir = normalize(viewPos - FragPos);
    //phong
    //vec3 reflectDir = reflect(-lightDir, norm);
    //float spec = pow(max(dot(viewDir, reflectDir), 0.0), 8.0);
    //Blinn-Phong
    vec3 halfwayDir= normalize(lightDir+viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), shininess);
    vec3 specular = mat_specular* spec * light.specular;
    
    vec3 result =ambient+diffuse+specular;
    FragColor =  vec4(result, 1.0);
}