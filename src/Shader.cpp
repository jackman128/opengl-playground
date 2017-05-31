#include "Shader.hpp"
#include <map>
#include <glm/gtc/type_ptr.hpp>

using namespace boost::filesystem;

Shader::Shader(const std::vector<std::string> &fnames) {
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
  this->Program = 0;
  Reload();
}

Shader::~Shader() {
  for (ShaderSrc shader : Sources) {
    glDeleteShader(shader.id);
  }
  glDeleteProgram(this->Program);
}

void Shader::Use() {
  glUseProgram(this->Program);
}

GLint Shader::CompileSrc(const path &p, GLenum type) {
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
    delete buffer;
    return 0;
  }
  else {
    delete buffer;
    return shaderId;
  }
}

void Shader::Reload() {
  std::stack<ShaderSrc*> newShaders;
  bool relink = false;
  GLint newProgram = 0;
  for (size_t i = 0; i < Sources.size(); i++) {
    ShaderSrc *s = &Sources[i];
    path p(s->fname);
    if (!exists(p))
      std::cerr << "bad";
    if (!is_regular_file(p))
      std::cerr << "bad";
    if (s->lastWrite != last_write_time(p)) {
      newShaders.push(s);
      relink = true;
    }
  }

  while (!newShaders.empty()) {
    ShaderSrc *s = newShaders.top();
    newShaders.pop();
    path p(s->fname);
    if (s->lastWrite != 0) {
      glDetachShader(this->Program, s->id);
      glDeleteShader(s->id);
    }
    s->lastWrite = last_write_time(p);
    GLint newShaderObj = CompileSrc(p, s->type);
    if (newShaderObj == 0) {
      relink = false;
    }
    else {
      if (newProgram == 0) newProgram = glCreateProgram();
      s->id = newShaderObj;
      glAttachShader(newProgram, s->id);
    }
  }

  if (relink) {
    GLchar buf[512];
    GLint success;
    glLinkProgram(newProgram);
    glGetProgramiv(newProgram, GL_LINK_STATUS, &success);
    if (success != GL_TRUE) {
      GLint length = 0;
      glGetProgramInfoLog(newProgram, 512, &length, buf);
      std::cerr.write(buf, length);
    }
    else {
      if (this->Program != 0) glDeleteProgram(this->Program);
      this->Program = newProgram;
      InitUniforms();
    }
  }
}

void Shader::InitUniforms() {
  Uniforms.clear();
  GLint numUniforms, length;
  GLchar buffer[256];
  GLenum type;
  glGetProgramiv(this->Program, GL_ACTIVE_UNIFORMS, &numUniforms);

  for (GLint i = 0; i < numUniforms; i++) {
    Uniform uni;
    glGetActiveUniform(this->Program, i, 256, NULL, &length, &type, buffer);
    uni.location = glGetUniformLocation(this->Program, buffer);
    uni.type = type;
    Uniforms.insert(std::pair<std::string, Uniform>(std::string(buffer), uni));
    std::cerr << uni.location << ":" << uni.type << ":" << buffer << std::endl;
  }
}

void Shader::SetUniform(const std::string &name, float v0){
    glProgramUniform1f(this->Program, Uniforms[name].location, v0);
}

void Shader::SetUniform(const std::string &name, const glm::vec3 &vec) {
    glProgramUniform3f(this->Program, Uniforms[name].location, vec.x, vec.y, vec.z);
}

void Shader::SetUniform(const std::string &name, const glm::mat4 &mat) {
    glProgramUniformMatrix4fv(this->Program, Uniforms[name].location, 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::SetUniform(const std::string &name, int v0) {
  glProgramUniform1i(this->Program, Uniforms[name].location, v0);
}

void Shader::SetUniform(const std::string &name, unsigned int v0) {
  glProgramUniform1ui(this->Program, Uniforms[name].location, v0);
}
