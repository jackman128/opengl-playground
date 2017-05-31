#version 330 core

#define NUM_POINTLIGHTS 4

in vec3 Normal;
in vec3 FragPos;

struct DirLight {
  vec3 direction;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

out vec4 Color;

uniform vec3 viewPos;
uniform DirLight dirLight;

void main()
{
  vec3 ambient = vec3(0.2125f, 0.1275f, 0.054f);
  vec3 lightDir = normalize(dirLight.direction - FragPos);
  vec3 norm = normalize(Normal);
  float diff = max(dot(norm, lightDir), 0.0);
  vec3 diffuse = (diff * vec3(0.714f, 0.4284f, 0.18144f));

  vec3 viewDir = normalize(viewPos - FragPos);
  vec3 reflectDir = reflect(-lightDir, norm);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0f), 0.2f * 128.0f);
  vec3 specular = (spec * vec3(0.393548f, 0.271906f, 0.166721f));

  Color = vec4(ambient+diffuse+specular, 1.0f);
}

