#version 330 core

struct Material {
    sampler2D diffuse;
    sampler2D specular;    
    float shininess;
}; 

struct PointLight {
    bool on;
    vec3 position;  
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
	
    float constant;
    float linear;
    float quadratic;
};

struct SpotLight {
    bool on;
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
  
    float constant;
    float linear;
    float quadratic;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;       
};

vec3 CalculatePointLight(vec3 norm, vec3 viewDir);
vec3 CalculateSpotLight(SpotLight spotLight, vec3 norm, vec3 viewDir);

in vec3 chFragPos;
in vec3 chNor;
in vec2 chTex;

out vec4 outCol;

uniform vec3 uViewPos;
uniform Material uMaterial;
uniform PointLight uPointLight;
uniform SpotLight uSpotLightAngel;
uniform SpotLight uFlashLight;

void main()
{
    vec3 norm = normalize(chNor);
    vec3 viewDir = normalize(uViewPos - chFragPos);

    vec3 result = CalculateSpotLight(uSpotLightAngel, norm, viewDir); 

    if (uPointLight.on)
    {
       result += CalculatePointLight(norm, viewDir);
    }
   
    if (uFlashLight.on)
    {
        result += CalculateSpotLight(uFlashLight, norm, viewDir); 
    }

	outCol = vec4(result, 1.0);
}

vec3 CalculatePointLight(vec3 norm, vec3 viewDir)
{
    vec3 ambient = uPointLight.ambient * texture(uMaterial.diffuse, chTex).rgb;

    vec3 lightDir = normalize(uPointLight.position - chFragPos); 
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = uPointLight.diffuse * diff * texture(uMaterial.diffuse, chTex).rgb;

    vec3 halfwayDir = normalize(lightDir + viewDir);
    //vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(norm, halfwayDir), 0.0), uMaterial.shininess);
    vec3 specular = uPointLight.specular * spec * texture(uMaterial.specular, chTex).rgb;
    
    float distance    = length(uPointLight.position - chFragPos);
    float attenuation = 1.0 / (uPointLight.constant + uPointLight.linear * distance + uPointLight.quadratic * (distance * distance));

    ambient  *= attenuation;  
    diffuse   *= attenuation;
    specular *= attenuation; 
    vec3 result = ambient + diffuse + specular;
    return result;
}

vec3 CalculateSpotLight(SpotLight light, vec3 norm, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - chFragPos);

    float diff = max(dot(norm, lightDir), 0.0);

    vec3 halfwayDir = normalize(lightDir + viewDir); 
    //vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), uMaterial.shininess);

    float distance = length(light.position - chFragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    

    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    vec3 ambient = light.ambient * vec3(texture(uMaterial.diffuse, chTex));
    vec3 diffuse = light.diffuse * diff * vec3(texture(uMaterial.diffuse, chTex));
    vec3 specular = light.specular * spec * vec3(texture(uMaterial.specular, chTex));
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}