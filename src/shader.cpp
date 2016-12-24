#include "shader.hpp"
Shader::Shader(const std::string vertexPath, const std::string fragmentPath) {
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
}

void Shader::Use() {
  glUseProgram(this->Program);
}
