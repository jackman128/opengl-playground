#include "Material.hpp"
#include <glm/gtc/type_ptr.hpp>

Material::Material(const std::string &vertexPath, const std::string &fragmentPath) {
  std::ifstream vertFin, fragFin;
  std::string vertSource, fragSource;

  vertFin.exceptions(std::ifstream::badbit);
  fragFin.exceptions(std::ifstream::badbit);

  try {
    vertFin.open(vertexPath);
    fragFin.open(fragmentPath);

    std::stringstream vertStream, fragStream;

    vertStream << vertFin.rdbuf();
    fragStream << fragFin.rdbuf();

    vertFin.close();
    fragFin.close();

    vertSource = vertStream.str();
    fragSource = fragStream.str();
  }
  catch(std::ifstream::failure e) {
    std::cerr << "Shader could not be successfully read." << std::endl;
  }

  const GLchar* vertCode = vertSource.c_str();
  const GLchar* fragCode = fragSource.c_str();

  GLuint vertex, fragment;
  GLint success;
  GLchar info[512];

  //compile vertex shader
  vertex = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex, 1, &vertCode, NULL);
  glCompileShader(vertex);
  //check for errors
  glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertex, 512, NULL, info);
    std::cerr << "Vertex shader compilation failed:\n" << info << std::endl;
  }

  //compile fragment shader
  fragment = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment, 1, &fragCode, NULL);
  glCompileShader(fragment);
  //check for errors
  glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragment, 512, NULL, info);
    std::cerr << "Fragment shader compilation failed:\n" << info << std::endl;
  }

  this->Program = glCreateProgram();
  glAttachShader(this->Program, vertex);
  glAttachShader(this->Program, fragment);
  glLinkProgram(this->Program);
  //check for errors
  glGetProgramiv(this->Program, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(this->Program, 512, NULL, info);
    std::cerr << "Shader program linking failed:\n" << info << std::endl;
  }

  //delete shaders
  glDeleteShader(vertex);
  glDeleteShader(fragment);

  InitUniforms();
}

Material::~Material() {
  glDeleteProgram(this->Program);
}

void Material::Use() {
  glUseProgram(this->Program);
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
    throw "error";
  glProgramUniform1f(this->Program, Uniforms[name].location, v0);
}

void Material::SetUniform(const std::string &name, const glm::vec3 &vec) {
  if (Uniforms[name].type != GL_FLOAT_VEC3)
    throw "error";
  glProgramUniform3f(this->Program, Uniforms[name].location, vec.x, vec.y, vec.z);
}

void Material::SetUniform(const std::string &name, const glm::mat4 &mat) {
  if (Uniforms[name].type != GL_FLOAT_MAT4)
    throw "error";
  glProgramUniformMatrix4fv(this->Program, Uniforms[name].location, 1, GL_FALSE, glm::value_ptr(mat));
}
