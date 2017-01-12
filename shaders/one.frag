#version 330 core

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

struct Material {
  vec3 specular;
  float shininess;
};

struct Light {
  vec3 position;

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
uniform Light light;

void main()
{
  float distance = length(light.position - FragPos);
  float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * pow(distance, 2.0f));
  //ambient
  vec3 ambient = light.ambient * vec3(texture(diffuse, TexCoords));
  ambient *= attenuation;

  //diffuse
  vec3 norm = normalize(Normal);
  vec3 lightDir = normalize(light.position - FragPos);
  float diff = max(dot(norm, lightDir), 0.0); //gives difference between angles of 'norm' and 'lightDir'
  vec3 diffuse = light.diffuse * diff * vec3(texture(diffuse, TexCoords));
  diffuse *= attenuation;

  //specular
  vec3 viewDir = normalize(viewPos - FragPos);
  vec3 reflectDir = reflect(-lightDir, norm);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
  vec3 specular = light.specular * spec * vec3(texture(specular, TexCoords));
  specular *= attenuation;

  gl_FragColor = vec4((ambient + diffuse + specular), 1.0);
}
