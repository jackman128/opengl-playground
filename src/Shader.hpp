#pragma once

#include <string>
#include <vector>
#include <map>

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include <GL/glew.h>
#include <glm/glm.hpp>

class Shader {
public:
  //GL program ID
  GLuint Program;

  //constructor
  Shader(const std::vector<std::string> &shaders);

  //destructor
  ~Shader();

  void AddShader(const std::string &shader);

  void Reload();

  //use shader program
  void Use();

  void SetUniform(const std::string &name, float v0);
  void SetUniform(const std::string &name, const glm::vec3 &vec);
  void SetUniform(const std::string &name, const glm::mat4 &mat);
  void SetUniform(const std::string &name, int v0);
  void SetUniform(const std::string &name, unsigned int v0);

private:
  GLint CompileSrc(const boost::filesystem::path &p, GLenum type);
  void LinkProgram();
  void InitUniforms();

  struct Uniform {
    GLint location = -1;
    GLenum type;
  };

  std::map<std::string, Uniform> Uniforms;

  struct ShaderSrc {
    std::string fname;
    unsigned int lastWrite = 0u;
    GLenum type;
    GLint id = 0u;
  };

  std::vector<ShaderSrc> Sources;
};

