#include "Material.hpp"
#include <map>
#include <glm/gtc/type_ptr.hpp>

using namespace boost::filesystem;



Material::Material(const std::vector<std::string> &fnames) {
  for (std::string fname : fnames) {
    ShaderSrc src;
    path p(fname);
    if (!exists(p))
      std::cerr << "file doesn't exist";
    if (!is_regular_file(p))
      std::cerr << "file isn't regular";
    std::string ext = extension(p);
    if (ext == ".vert")
      src.type = GL_VERTEX_SHADER;
    else if (ext == ".geom")
      src.type = GL_GEOMETRY_SHADER;
    else if (ext == ".tesc")
      src.type = GL_TESS_CONTROL_SHADER;
    else if (ext == ".tese")
      src.type = GL_TESS_EVALUATION_SHADER;
    else if (ext == ".frag")
      src.type = GL_FRAGMENT_SHADER;
    else if (ext == ".comp")
      src.type = GL_COMPUTE_SHADER;
    else
      std::cerr << "extension not recognized";
    src.fname = fname;
    Sources.push_back(src);
  }
  this->Program = glCreateProgram();
  Reload();
}

Material::~Material() {
  for (ShaderSrc shader : Sources) {
    glDeleteShader(shader.id);
  }
  glDeleteProgram(this->Program);
}

void Material::Use() {
  glUseProgram(this->Program);
}

GLint Material::CompileSrc(const path &p, GLenum type) {
  size_t fsize = file_size(p);
  char *buffer = new char[(fsize > 512) ? fsize : 512];

  ifstream fin(p);
  fin.read(buffer, fsize);
  fin.close();
 
  //compile vertex shader
  GLint success;
  GLint size = (int)fsize;
  GLuint shaderId;
  shaderId = glCreateShader(type);
  glShaderSource(shaderId, 1, &buffer, &size);
  glCompileShader(shaderId);
  //check for errors
  glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
  if (success != GL_TRUE) {
    GLint length = 0;
    glGetShaderInfoLog(shaderId, 512, &length, buffer);
    std::cerr.write(buffer, length);
  }
  return shaderId;
}

void Material::Reload() {
  std::vector<int> newShaderIndex;
  for (unsigned int i = 0; i < Sources.size(); i++) {
    path p(Sources[i].fname);
    if (!exists(p))
      std::cerr << "bad";
    if (!is_regular_file(p))
      std::cerr << "bad";
    if (Sources[i].lastWrite == last_write_time(p))
      continue;
    else
      newShaderIndex.push_back(i);
  }

  for (unsigned int j = 0; j < newShaderIndex.size(); j++) {
    ShaderSrc s = Sources[newShaderIndex[j]];
    path p(s.fname);
    if (s.lastWrite != 0) {
      glDetachShader(this->Program, s.id);
      glDeleteShader(s.id);
    }
    Sources[newShaderIndex[j]].id = CompileSrc(p, s.type);
    glAttachShader(this->Program, Sources[newShaderIndex[j]].id);
    Sources[newShaderIndex[j]].lastWrite = last_write_time(p);
  }

  if (newShaderIndex.size() > 0) {
    GLchar buf[512];
    GLint success;
    glLinkProgram(this->Program);
    glGetProgramiv(this->Program, GL_LINK_STATUS, &success);
    if (success != GL_TRUE) {
      GLint length = 0;
      glGetProgramInfoLog(this->Program, 512, &length, buf);
      std::cerr.write(buf, length);
    }
    InitUniforms();
  }

}

void Material::InitUniforms() {
  GLint numUniforms, uniLocation, length, buffSize = 256;
  GLchar buffer[256];
  GLenum type;
  glGetProgramiv(this->Program, GL_ACTIVE_UNIFORMS, &numUniforms);

  for (GLint i = 0; i < numUniforms; i++) {
    glGetActiveUniform(this->Program, i, buffSize, NULL, &length, &type, buffer);
    uniLocation = glGetUniformLocation(this->Program, buffer);
    Uniform uni;
    uni.type = type;
    uni.location = uniLocation;
    Uniforms.insert(std::pair<std::string, Uniform>(std::string(buffer), uni));
  }
}

void Material::SetUniform(const std::string &name, float v0){
  if (Uniforms[name].type != GL_FLOAT)
    std::cerr << "error" << __LINE__ << __FILE__ << std::endl;
  glProgramUniform1f(this->Program, Uniforms[name].location, v0);
}

void Material::SetUniform(const std::string &name, const glm::vec3 &vec) {
  if (Uniforms[name].type != GL_FLOAT_VEC3)
    std::cerr << "error" << __LINE__ << __FILE__ << std::endl;
  glProgramUniform3f(this->Program, Uniforms[name].location, vec.x, vec.y, vec.z);
}

void Material::SetUniform(const std::string &name, const glm::mat4 &mat) {
  if (Uniforms[name].type != GL_FLOAT_MAT4)
    std::cerr << "error" << __LINE__ << __FILE__ << std::endl;
  glProgramUniformMatrix4fv(this->Program, Uniforms[name].location, 1, GL_FALSE, glm::value_ptr(mat));
}
