#include "Model.hpp"
#include <SOIL2.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

GLint TextureFromFile(const char* path, string directory);

Model::Model(GLchar* path)
{
  this->loadModel(path);
}

void Model::Draw(Shader shader)
{
  for (GLuint i = 0; i < this->meshes.size(); i++)
    this->meshes[i].Draw(shader);
}

void Model::loadModel(string path)
{
  Assimp::Importer import;
  const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

  //fail if either the scene or the root node are null, or if the scene data is incomplete
  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
    cout << "Assimp error: " << import.GetErrorString() << endl;
    return;
  }
  this->directory = path.substr(0, path.find_last_of("/"));
  this->processNode(scene->mRootNode, scene);
}

//recursve function that puts every single node in the aiScene into the meshes vector.
void Model::processNode(aiNode* node, const aiScene* scene)
{
  for (GLuint i = 0; i < node->mNumMeshes; i++) {
    aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
    this->meshes.push_back(this->processMesh(mesh, scene));
  }

  for (GLuint i = 0; i < node->mNumChildren; i++) {
    this->processNode(node->mChildren[i], scene);
  }
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
  vector<Vertex> vertices;
  vector<GLuint> indices;
  vector<Texture> textures;

  for (GLuint i = 0; i < mesh->mNumVertices; i++) {
    Vertex vertex;
    glm::vec3 vector;
    vector.x = mesh->mVertices[i].x;
    vector.y = mesh->mVertices[i].y;
    vector.z = mesh->mVertices[i].z;
    vertex.Position = vector;

    vector.x = mesh->mNormals[i].x;
    vector.y = mesh->mNormals[i].y;
    vector.z = mesh->mNormals[i].z;
    vertex.Normal = vector;

    if (mesh->mTextureCoords[0]) {
      glm::vec2 vec;
      vec.x = mesh->mTextureCoords[0][i].x;
      vec.y = mesh->mTextureCoords[0][i].y;
      vertex.TexCoords = vec;
    }
    else
      vertex.TexCoords = glm::vec2(0.0f, 0.0f);

    vertices.push_back(vertex);
  }

  for (GLuint i = 0; i < mesh->mNumFaces; i++) {
    aiFace face = mesh->mFaces[i];
    for (GLuint j = 0; j < face.mNumIndices; j++) {
      indices.push_back(face.mIndices[j]);
    }
  }

  if (mesh->mMaterialIndex >= 0) {
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    vector<Texture> diffuseMaps = this->loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    vector<Texture> specularMaps = this->loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

  }
  return Mesh(vertices, indices, textures);
}

vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName)
{
  vector<Texture> textures;
  for (GLuint i = 0; i < mat->GetTextureCount(type); i++) {
    aiString str;
    mat->GetTexture(type, i, &str);
    bool skip = false;
    for (unsigned int j = 0; j < texturesLoaded.size(); j++) {
      if (strcmp(texturesLoaded[j].path.C_Str(), str.C_Str()) == 0) {
        textures.push_back(texturesLoaded[j]);
        skip = true;
        break;
      }
    }
    if (!skip) {
      Texture texture;
      texture.id = TextureFromFile(str.C_Str(), this->directory);
      texture.type = typeName;
      texture.path = str;
      textures.push_back(texture);
      this->texturesLoaded.push_back(texture);
    }
  }
  return textures;
}

GLint TextureFromFile(const char* path, string directory)
{
  string filename = string(path);
  filename = directory + '/' + filename;
  cout << "Texture path: " << filename << endl;
  GLuint textureID = SOIL_load_OGL_texture(filename.c_str(),
                                               SOIL_LOAD_AUTO,
                                               SOIL_CREATE_NEW_ID,
                                               SOIL_FLAG_MIPMAPS);
  if (!textureID) {
    cout << "SOIL error: " << SOIL_last_result() << endl;
  }

  // Parameters
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, 0);

  return textureID;
}
