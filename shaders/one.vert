#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texcoords;

out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoords;

struct Light {
  vec3 position;
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

uniform Light light;
uniform mat4 view;
uniform mat4 proj;
uniform mat4 model;

void main()
{
    FragPos = vec3(model * vec4(position, 1.0f));
    Normal = mat3(transpose(inverse(model))) * normal;
    TexCoords = texcoords;
    gl_Position = proj * view * model * vec4(position, 1.0f);
}
