#version 330 core

struct Material {
    sampler2D diffuse;
    sampler2D specular;    
    float shininess;
}; 

struct PointLight {
    vec3 position;  
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
	
    float constant;
    float linear;
    float quadratic;
};

in vec3 chFragPos;
in vec3 chNor;
in vec2 chTex;

out vec4 outCol;

uniform vec3 uViewPos;
uniform Material uMaterial;
uniform PointLight uPointLight;

void main()
{
    vec3 ambient = uPointLight.ambient * texture(uMaterial.diffuse, chTex).rgb;

    vec3 norm = normalize(chNor);
    vec3 lightDir = normalize(uPointLight.position - chFragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = uPointLight.diffuse * diff * texture(uMaterial.diffuse, chTex).rgb;

    vec3 viewDir = normalize(uViewPos - chFragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), uMaterial.shininess);
    vec3 specular = uPointLight.specular * spec * texture(uMaterial.specular, chTex).rgb;
    
    float distance    = length(uPointLight.position - chFragPos);
    float attenuation = 1.0 / (uPointLight.constant + uPointLight.linear * distance + uPointLight.quadratic * (distance * distance));

    ambient  *= attenuation;  
    diffuse   *= attenuation;
    specular *= attenuation; 
    vec3 result = ambient + diffuse + specular;

	outCol = vec4(result, 1.0);
}