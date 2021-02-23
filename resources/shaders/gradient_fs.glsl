#version 330

out vec4 FragColor;

in vec3 FragPos;  

uniform float maximum;
uniform float minimum;

void main()
{
    vec3 color1 = vec3(0,1,0);
    vec3 color2 = vec3(1,1,0);
    vec3 color3 = vec3(1,0,0);

    if(FragPos.y<=(maximum-minimum)/2+minimum)
    {
        vec3 color = mix(color1,color2, (FragPos.y-minimum) /( ( maximum-minimum)/2 ) );

        FragColor = vec4(color,1.0);
    }
    else
    {
        vec3 color = mix(color2,color3,(FragPos.y-minimum-(maximum-minimum)/2 ) / ( (maximum-minimum)/2 ) );

        FragColor = vec4(color,1.0);
    }
}