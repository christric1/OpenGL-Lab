#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

struct Material{
    vec3 ambient;
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct LightDirectional{
    vec3 pos;
	vec3 color;
	vec3 dirToLight;
};

struct LightPoint{
    vec3 pos;
	vec3 color;
	vec3 dirToLight;    // µL¥Î
    
    float constant;
	float linear;
	float quadratic;
};

struct LightSpot{
    vec3 pos;
	vec3 color;
	vec3 dirToLight;

    float constant;
	float linear;
	float quadratic;

    float cosPhyInner;
	float cosPhyOutter;
};

uniform Material material;
uniform LightDirectional lightD;
uniform LightPoint lightP0;
uniform LightPoint lightP1;
uniform LightPoint lightP2;
uniform LightPoint lightP3;
uniform LightSpot lightS;

uniform vec3 cameraPos;

vec3 CalcLightDirection(LightDirectional light, vec3 uNormal, vec3 dirToCamera){
    
    // diffuse
    float diffIntensity = max(dot(light.dirToLight, uNormal), 0);
    vec3 diffColor = diffIntensity * light.color * texture(material.diffuse, TexCoord).rgb;
    
    // specular
    vec3 R = normalize(reflect(-light.dirToLight, uNormal));
    float specIntensity = pow(max(dot(R, dirToCamera), 0), material.shininess);
    vec3 speColor = specIntensity * light.color * texture(material.specular, TexCoord).rgb;

    vec3 result = diffColor + speColor;
    return result;
}

vec3 CalcLightPoint(LightPoint light, vec3 uNormal, vec3 dirToCamera){

    // attenuation
    float distance = length(light.pos - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
                    	light.quadratic * (distance * distance));

    // diffuse
    float diffIntensity = max(dot(normalize(light.pos - FragPos), uNormal), 0) * attenuation;
    vec3 diffColor = diffIntensity * light.color * texture(material.diffuse, TexCoord).rgb;

    // specular
    vec3 R = normalize(reflect(-normalize(light.pos - FragPos), uNormal));
    float specIntensity = pow(max(dot(R, dirToCamera), 0), material.shininess) * attenuation;
    vec3 speColor = specIntensity * light.color * texture(material.specular, TexCoord).rgb;

    vec3 result = diffColor + speColor;

    return result;
}

vec3 CalcLightSpot(LightSpot light, vec3 uNormal, vec3 dirToCamera){

    // attenuation
    float distance = length(light.pos - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
                    	light.quadratic * (distance * distance));
    float spotRatio;
    float cosTheta = dot(normalize(FragPos - light.pos), -light.dirToLight);

    if(cosTheta > light.cosPhyInner){  // inside
        spotRatio = 1.0;
    }
    else if(cosTheta > light.cosPhyOutter){    // middle
        spotRatio = (cosTheta - light.cosPhyOutter)/(light.cosPhyInner - light.cosPhyOutter);
    }
    else{   // outside
        spotRatio = 0.0;
    }

    // diffuse
    float diffIntensity = max(dot(normalize(light.pos - FragPos), uNormal), 0) * attenuation * spotRatio;
    vec3 diffColor = diffIntensity * light.color * texture(material.diffuse, TexCoord).rgb;

    // specular
    vec3 R = normalize(reflect(-normalize(light.pos - FragPos), uNormal));
    float specIntensity = pow(max(dot(R, dirToCamera), 0), material.shininess) * attenuation * spotRatio;
    vec3 speColor = specIntensity * light.color * texture(material.specular, TexCoord).rgb;

    vec3 result = diffColor + speColor;
    return result;
}

void main(){
    
    vec3 finalResult = vec3(0, 0, 0);
    vec3 uNormal = normalize(Normal);
    vec3 dirToCamera = normalize(cameraPos - FragPos);

    finalResult += CalcLightDirection(lightD, uNormal, dirToCamera);
    finalResult += CalcLightPoint(lightP0, uNormal, dirToCamera);
    finalResult += CalcLightPoint(lightP1, uNormal, dirToCamera);
    finalResult += CalcLightPoint(lightP2, uNormal, dirToCamera);
    finalResult += CalcLightPoint(lightP3, uNormal, dirToCamera);
    finalResult += CalcLightSpot(lightS, uNormal, dirToCamera);

    FragColor = vec4(finalResult, 1.0);

} 