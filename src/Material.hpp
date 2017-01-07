#ifndef MATERIAL_H
#define MATERIAL_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>

#include <GL/glew.h>
#include <glm/glm.hpp>

class Material {
public:
  //GL program ID
  GLuint Program;

  //constructor
  Material(const std::string &vertexPath, const std::string &fragmentPath);

  //destructor
  ~Material();

  //use shader program
  void Use();

  void SetUniform(const std::string &name, float v0);
  void SetUniform(const std::string &name, const glm::vec3 &vec);
  void SetUniform(const std::string &name, const glm::mat4 &mat);

private:
  void CompileShaders();

  struct Uniform {
    GLint location;
    GLenum type;
  };

  std::map<std::string, Uniform> Uniforms;

  void InitUniforms();
};

#endif
