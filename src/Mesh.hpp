#pragma once

#include <vector>
#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Shader.hpp"

using namespace std;
using namespace glm;

struct Vertex {
  vec3 Position;
  vec3 Normal;
  vec2 TexCoords;
};

struct Texture {
  GLuint id;
  string type;
  aiString path;
};

class Mesh {
public:
  vector<Vertex> vertices;
  vector<GLuint> indices;
  vector<Texture> textures;

  Mesh(vector<Vertex> vertices, vector<GLuint> indices, vector<Texture> textures);
  void Draw(Shader shader);
private:
  GLuint VAO, VBO, EBO;
  void setupMesh();
};

