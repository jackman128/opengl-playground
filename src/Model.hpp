#pragma once

#include <vector>
#include <string>
#include <GL/glew.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Mesh.hpp"
#include "Shader.hpp"

using namespace std;

class Model
{
public:
  Model(GLchar* path);
  void Draw(Shader shader);
private:
  vector<Mesh> meshes;
  vector<Texture> texturesLoaded;
  string directory;
  void loadModel(string path);
  void processNode(aiNode* node, const aiScene* scene);
  Mesh processMesh(aiMesh* mesh, const aiScene* scene);
  vector<Texture> loadMaterialTextures(aiMaterial* mat,aiTextureType type, string typeName);
};
