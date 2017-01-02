#version 330 core

in vec2 TexCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{
  gl_FragColor = texture(texture2, vec2(TexCoord.x, 1.0 - TexCoord.y));
}
