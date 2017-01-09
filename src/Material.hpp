#ifndef MATERIAL_H
#define MATERIAL_H

#include <string>
#include <map>
#include <vector>

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include <GL/glew.h>
#include <glm/glm.hpp>

class Material {
public:
  //GL program ID
  GLuint Program;

  //constructor
  Material(const std::vector<std::string> &shaders);

  //destructor
  ~Material();

  void AddShader(const std::string &shader);

  void Reload();

  //use shader program
  void Use();

  void SetUniform(const std::string &name, float v0);
  void SetUniform(const std::string &name, const glm::vec3 &vec);
  void SetUniform(const std::string &name, const glm::mat4 &mat);

private:
  GLint CompileSrc(const boost::filesystem::path &p, GLenum type);
  void LinkProgram();
  void InitUniforms();

  struct Uniform {
    GLint location;
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

#endif
