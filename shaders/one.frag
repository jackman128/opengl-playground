#version 330 core

in vec3 Normal;
in vec3 FragPos;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
  //ambient
  float ambientIntensity = 0.1f;
  vec3 ambient = ambientIntensity * lightColor;

  //diffuse
  vec3 norm = normalize(Normal);
  vec3 lightDir = normalize(lightPos - FragPos);
  float diff = max(dot(norm, lightDir), 0.0); //gives difference between angles of 'norm' and 'lightDir'
  vec3 diffuse = diff * lightColor;

  //specular
  float specularStrength = 0.5f;
  vec3 viewDir = normalize(viewPos - FragPos);
  vec3 reflectDir = reflect(-lightDir, norm);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
  vec3 specular = specularStrength * spec * objectColor;

  gl_FragColor = vec4((ambient + diffuse + specular) * objectColor, 1.0);
}
