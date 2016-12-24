#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <GL/glew.h>

class Shader {
public:
  //GL program ID
  GLuint Program;

  //constructor
  Shader(const std::string vertexPath, const std::string fragmentPath);

  //use shader program
  void Use();
};

#endif
