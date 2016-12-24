#version 330 core
  
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;

out vec3 myPosition;

uniform vec3 offset;

void main()
{
    gl_Position = vec4(position.x + offset.x, position.y + offset.y, position.z + offset.z, 1.0);
    myPosition = position;
}