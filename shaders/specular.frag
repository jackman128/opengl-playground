#version 330 core

in vec2 TexCoords;

uniform sampler2D specular;

void main() {
  gl_FragColor = vec4(texture(specular, TexCoords).rrr, 1.0);
}
