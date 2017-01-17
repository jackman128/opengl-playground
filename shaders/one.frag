#version 330 core

#define NUM_POINTLIGHTS 4

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

struct Material {
  vec3 specular;
  float shininess;
};

struct DirLight {
  vec3 direction;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
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

struct SpotLight {
  vec3 position;
  vec3 direction;

  float innerCutOff;
  float outerCutOff;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;

  float constant;
  float linear;
  float quadratic;
};

uniform sampler2D specular;
uniform sampler2D diffuse;
uniform vec3 viewPos;
uniform Material material;
uniform DirLight dirLight;
uniform PointLight pointLights[NUM_POINTLIGHTS];
uniform SpotLight spotLight;

vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
  vec3 norm = normalize(Normal);
  vec3 viewDir = normalize(viewPos - FragPos);

  vec3 result = calcSpotLight(spotLight, norm, FragPos, viewDir);

  result += calcDirLight(dirLight, norm, viewDir);

  for (int i = 0; i < NUM_POINTLIGHTS; i++) {
    result += calcPointLight(pointLights[i], norm, FragPos, viewDir);
  }

  gl_FragColor = vec4(result, 1.0);
}

vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir) {
  vec3 lightDir = normalize(-light.direction);

  float diff = max(dot(normal, lightDir), 0.0);
  vec3 reflectDir = reflect(-lightDir, normal);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

  vec3 ambient = light.ambient * texture(diffuse, TexCoords).rgb;
  vec3 diffuse = light.diffuse * diff * texture(diffuse, TexCoords).rgb;
  vec3 specular = light.specular * spec * texture(specular, TexCoords).rrr;

  return ambient + diffuse + specular;
}

vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
  vec3 lightDir = normalize(light.position - fragPos);
  float diff = max(dot(normal, lightDir), 0.0);
  vec3 reflectDir = reflect(lightDir, normal);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
  float distance = length(light.position - fragPos);
  float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * pow(distance, 2.0));
  //float attenuation = 1.0 / distance;

  vec3 ambient = light.ambient * texture(diffuse, TexCoords).rgb;
  vec3 diffuse = light.diffuse * diff * texture(diffuse, TexCoords).rgb;
  vec3 specular = light.specular * spec * texture(specular, TexCoords).rrr;
  ambient *= attenuation;
  diffuse *= attenuation;
  specular *= attenuation;

  return ambient + diffuse + specular;
}

vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
  vec3 lightDir = normalize(light.position - fragPos);
  // Diffuse shading
  float diff = max(dot(normal, lightDir), 0.0);
  // Specular shading
  vec3 reflectDir = reflect(-lightDir, normal);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
  // Attenuation
  float distance = length(light.position - fragPos);
  float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
  // Spotlight intensity
  float theta = dot(lightDir, normalize(-light.direction));
  float epsilon = light.innerCutOff - light.outerCutOff;
  float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
  // Combine results
  vec3 ambient = light.ambient * vec3(texture(diffuse, TexCoords));
  vec3 diffuse = light.diffuse * diff * vec3(texture(diffuse, TexCoords));
  vec3 specular = light.specular * spec * vec3(texture(specular, TexCoords));
  ambient *= attenuation * intensity;
  diffuse *= attenuation * intensity;
  specular *= attenuation * intensity;
  return vec3(ambient + diffuse + specular);
}
