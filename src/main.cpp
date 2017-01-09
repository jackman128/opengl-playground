#define GLEW_STATIC
#define GLM_ENABLE_EXPERIMENTAL
#include <iostream>
#include <fstream>
#include <exception>
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mouse.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>
#include <gli/gli.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include "context.hpp"
#include "Material.hpp"

bool initSDL(SDL_Window *&window, SDL_GLContext &context);
GLuint CreateTexture(char const* Filename);

const std::string vertexPath = "shaders/one.vert";
const std::string fragmentPath = "shaders/one.frag";
const std::string texture1Path = "textures/container.dds";
const std::string texture2Path = "textures/fish.dds";
const int windowWidth = 1800;
const int windowHeight = 900;
const GLfloat cameraSpeed = 5.0f;

const GLfloat vertices[] = {
  //vertex                surface normal
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
};

int main(int argc, char **argv) {
  glewExperimental = GL_TRUE;
  SDL_Window* window = NULL;
  SDL_GLContext context = NULL;
  if (!initSDL(window, context)) {
    std::cerr << "SDL init failed" << std::endl;
    return 0;
  }
  
  glewInit();
  glViewport(0, 0, windowWidth, windowHeight);
  GLint flags; glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
  if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
    {
      glEnable(GL_DEBUG_OUTPUT);
      glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); 
      glDebugMessageCallback(glDebugOutput, nullptr);
      glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    }
  
  GLuint vbo, vao, lightVao;
  glGenBuffers(1, &vbo);
  glGenVertexArrays(1, &vao);
  glGenVertexArrays(1, &lightVao);
  glBindVertexArray(vao);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  //position attrib
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
  glEnableVertexAttribArray(0);

  //surface normal attrib
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
  glEnableVertexAttribArray(1);

  glBindVertexArray(0);
  glEnableVertexAttribArray(0);
 
  Material shaderOne(vertexPath, fragmentPath);
  Material lampShader(vertexPath, "shaders/lamp.frag");

  /*
  //init texture
  GLuint texture1 = CreateTexture(texture1Path.c_str());
  if (texture1 == 0)
    throw "GLI goof";
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, 0);
  */

  //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glEnable(GL_MULTISAMPLE);
  glEnable(GL_DEPTH_TEST);
  //glEnable(GL_CULL_FACE);
  //glCullFace(GL_BACK);
  //glEnable (GL_BLEND);
  //glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glClearColor(0.0, 0.0, 0.0, 1.0);

  std::vector<glm::vec3> cubePositions = {
    glm::vec3( 0.0f,  0.0f,  0.0f),
    glm::vec3( 2.0f,  5.0f, -15.0f),
    glm::vec3(-1.5f, -2.2f, -2.5f),
    glm::vec3(-3.8f, -2.0f, -12.3f),
    glm::vec3( 2.4f, -0.4f, -3.5f),
    glm::vec3(-1.7f,  3.0f, -7.5f),
    glm::vec3( 1.3f, -2.0f, -2.5f),
    glm::vec3( 1.5f,  2.0f, -2.5f),
    glm::vec3( 1.5f,  0.2f, -1.5f),
    glm::vec3(-1.3f,  1.0f, -1.5f)
  };
  glm::vec3 lightPos(1.2f, 1.0f, 5.0f);
  glm::vec3 cameraPos(0.0f, 0.0f, 3.0f);
  glm::vec3 cameraFront(0.0f, 0.0f, -1.0f);
  glm::vec3 cameraUp(0.0f, 1.0f, 0.0f);
  glm::quat cameraQuat;

  SDL_Event windowEvent;

  unsigned int fpsLast = SDL_GetTicks();
  unsigned int fpsCurrent;
  unsigned int fpsFrames = 0;
  GLfloat deltaTime = 0.0f;
  GLfloat lastFrame = 0.0f;
  GLfloat fov = 45.0f;
  int mouseX, mouseY;
  mouseX = mouseY = 0;

  std::vector<bool> keys(256, false);

  while (1) {
    //events
    while (SDL_PollEvent(&windowEvent)) {
      if (windowEvent.type == SDL_QUIT) break;
      else if (windowEvent.type == SDL_KEYDOWN) {
        keys[windowEvent.key.keysym.scancode] = true;
      }
      else if (windowEvent.type == SDL_KEYUP) {
        keys[windowEvent.key.keysym.scancode] = false;
      }
      else if (windowEvent.type == SDL_MOUSEMOTION) {
        mouseX = windowEvent.motion.xrel;
        mouseY = windowEvent.motion.yrel;
      }
    }

    //update
    GLfloat time = SDL_GetTicks() / 1000.0f;
    deltaTime = time - lastFrame;
    lastFrame = time;
    //do movement
    if (keys[SDL_SCANCODE_W])
      cameraPos += (glm::conjugate(cameraQuat) * glm::vec3(0.0f, 0.0f, -1.0f)) * cameraSpeed * deltaTime;
    if (keys[SDL_SCANCODE_S])
      cameraPos -= (glm::conjugate(cameraQuat) * glm::vec3(0.0f, 0.0f, -1.0f)) * cameraSpeed * deltaTime;
    if (keys[SDL_SCANCODE_A])
      cameraPos += (glm::conjugate(cameraQuat) * glm::vec3(-1.0f, 0.0f, 0.0f)) * cameraSpeed * deltaTime;
    if (keys[SDL_SCANCODE_D])
      cameraPos -= (glm::conjugate(cameraQuat) * glm::vec3(-1.0f, 0.0f, 0.0f)) * cameraSpeed * deltaTime;
    if (keys[SDL_SCANCODE_Q])
      cameraPos -= (glm::conjugate(cameraQuat) * glm::vec3(0.0f, -1.0f, 0.0f)) * cameraSpeed * deltaTime;
    if (keys[SDL_SCANCODE_E])
      cameraPos += (glm::conjugate(cameraQuat) * glm::vec3(0.0f, -1.0f, 0.0f)) * cameraSpeed * deltaTime;
    if (keys[SDL_SCANCODE_ESCAPE])
      break;

    glm::vec2 mouseDelta(mouseX, mouseY);
    mouseX = mouseY = 0;
    float yaw = 0.002f * mouseDelta.x;
    float pitch = 0.002f * mouseDelta.y;
    float roll = 0.0f;
    glm::quat cameraOrient(glm::vec3(pitch, yaw, roll));
    cameraQuat = cameraOrient * cameraQuat;
    cameraQuat = glm::normalize(cameraQuat);
    glm::mat4 rotate = glm::mat4_cast(cameraQuat);
    glm::mat4 translate(1.0f);
    translate = glm::translate(translate, -cameraPos);
    glm::mat4 view = rotate * translate;

    lightPos.x = sin(time) * 5;

    //render
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /*
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glUniform1i(glGetUniformLocation(shaderOne.Program, "texture1"), 0);
    */

    shaderOne.Use();

    shaderOne.SetUniform("viewPos", cameraPos);


    shaderOne.SetUniform("light.position", lightPos);
    shaderOne.SetUniform("light.ambient", glm::vec3(0.2));
    shaderOne.SetUniform("light.diffuse", glm::vec3(1.0));
    shaderOne.SetUniform("light.specular", glm::vec3(1.0));

    shaderOne.SetUniform("material.ambient", glm::vec3(0.0, 	0.1, 	0.06));
    shaderOne.SetUniform("material.diffuse", glm::vec3(0.0, 	0.50980392, 	0.50980392));
    shaderOne.SetUniform("material.specular", glm::vec3(0.50196078, 	0.50196078, 	0.50196078));
    shaderOne.SetUniform("material.shininess", 0.25 * 128.0f);
      
    glm::mat4 proj = glm::perspective(fov, (GLfloat)windowWidth / (GLfloat)windowHeight, 0.1f, 100.0f);
    shaderOne.SetUniform("view", view);
    shaderOne.SetUniform("proj", proj);

    glBindVertexArray(vao);
    for(int i = 0; i < cubePositions.size(); i++) {
      glm::mat4 model;
      model = glm::translate(model, cubePositions[i]);
      shaderOne.SetUniform("model", model);
      glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    lampShader.Use();
    glm::mat4 model;
    model = glm::translate(model, lightPos);
    model = glm::scale(model, glm::vec3(0.2f));
    lampShader.SetUniform("model", model);
    lampShader.SetUniform("view", view);
    lampShader.SetUniform("proj", proj);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    SDL_GL_SwapWindow(window);

    fpsFrames++;
    if (fpsLast < SDL_GetTicks() - 1.0f * 1000) {
      fpsLast = SDL_GetTicks();
      fpsCurrent = fpsFrames;
      fpsFrames = 0;
      std::cout << "FPS: " << fpsCurrent << std::endl;
    }
  }
  
  //Clean Up
  glDeleteVertexArrays(1, &vao);
  glDeleteBuffers(1, &vbo);
  SDL_GL_DeleteContext(context);
  context = NULL;
  SDL_DestroyWindow(window);
  window = NULL;
  SDL_Quit();
  return 0;
}

bool initSDL(SDL_Window *&window, SDL_GLContext &context) {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cout << "SDL goofed: " << SDL_GetError() << std::endl;
    return false;
  }
  else {
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
#ifndef NDEBUG
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif //NDEBUG
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 2);
    SDL_SetRelativeMouseMode(SDL_TRUE);
    window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              windowWidth, windowHeight, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
    if (window == NULL) {
      std::cout << "window goof" << std::endl;
      return false;
    }
    context = SDL_GL_CreateContext(window);
    SDL_GL_SetSwapInterval(1);
    return true;
  }
}

GLuint CreateTexture(char const* Filename)
{
    gli::texture Texture = gli::load(Filename);
    if(Texture.empty())
        return 0;

    gli::gl GL(gli::gl::PROFILE_GL33);
    gli::gl::format const Format = GL.translate(Texture.format(), Texture.swizzles());
    GLenum Target = GL.translate(Texture.target());

    GLuint TextureName = 0;
    glGenTextures(1, &TextureName);
    glBindTexture(Target, TextureName);
    glTexParameteri(Target, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(Target, GL_TEXTURE_MAX_LEVEL, static_cast<GLint>(Texture.levels() - 1));
    glTexParameteri(Target, GL_TEXTURE_SWIZZLE_R, Format.Swizzles[0]);
    glTexParameteri(Target, GL_TEXTURE_SWIZZLE_G, Format.Swizzles[1]);
    glTexParameteri(Target, GL_TEXTURE_SWIZZLE_B, Format.Swizzles[2]);
    glTexParameteri(Target, GL_TEXTURE_SWIZZLE_A, Format.Swizzles[3]);

    glm::tvec3<GLsizei> const Extent(Texture.extent());
    GLsizei const FaceTotal = static_cast<GLsizei>(Texture.layers() * Texture.faces());

    switch(Texture.target())
    {
    case gli::TARGET_1D:
        glTexStorage1D(
            Target, static_cast<GLint>(Texture.levels()), Format.Internal, Extent.x);
        break;
    case gli::TARGET_1D_ARRAY:
    case gli::TARGET_2D:
    case gli::TARGET_CUBE:
        glTexStorage2D(
            Target, static_cast<GLint>(Texture.levels()), Format.Internal,
            Extent.x, Texture.target() == gli::TARGET_2D ? Extent.y : FaceTotal);
        break;
    case gli::TARGET_2D_ARRAY:
    case gli::TARGET_3D:
    case gli::TARGET_CUBE_ARRAY:
        glTexStorage3D(
            Target, static_cast<GLint>(Texture.levels()), Format.Internal,
            Extent.x, Extent.y,
            Texture.target() == gli::TARGET_3D ? Extent.z : FaceTotal);
        break;
    default:
        assert(0);
        break;
    }

    for(std::size_t Layer = 0; Layer < Texture.layers(); ++Layer)
    for(std::size_t Face = 0; Face < Texture.faces(); ++Face)
    for(std::size_t Level = 0; Level < Texture.levels(); ++Level)
    {
        GLsizei const LayerGL = static_cast<GLsizei>(Layer);
        glm::tvec3<GLsizei> Extent(Texture.extent(Level));
        Target = gli::is_target_cube(Texture.target())
            ? static_cast<GLenum>(GL_TEXTURE_CUBE_MAP_POSITIVE_X + Face)
            : Target;

        switch(Texture.target())
        {
        case gli::TARGET_1D:
            if(gli::is_compressed(Texture.format()))
                glCompressedTexSubImage1D(
                    Target, static_cast<GLint>(Level), 0, Extent.x,
                    Format.Internal, static_cast<GLsizei>(Texture.size(Level)),
                    Texture.data(Layer, Face, Level));
            else
                glTexSubImage1D(
                    Target, static_cast<GLint>(Level), 0, Extent.x,
                    Format.External, Format.Type,
                    Texture.data(Layer, Face, Level));
            break;
        case gli::TARGET_1D_ARRAY:
        case gli::TARGET_2D:
        case gli::TARGET_CUBE:
            if(gli::is_compressed(Texture.format()))
                glCompressedTexSubImage2D(
                    Target, static_cast<GLint>(Level),
                    0, 0,
                    Extent.x,
                    Texture.target() == gli::TARGET_1D_ARRAY ? LayerGL : Extent.y,
                    Format.Internal, static_cast<GLsizei>(Texture.size(Level)),
                    Texture.data(Layer, Face, Level));
            else
                glTexSubImage2D(
                    Target, static_cast<GLint>(Level),
                    0, 0,
                    Extent.x,
                    Texture.target() == gli::TARGET_1D_ARRAY ? LayerGL : Extent.y,
                    Format.External, Format.Type,
                    Texture.data(Layer, Face, Level));
            break;
        case gli::TARGET_2D_ARRAY:
        case gli::TARGET_3D:
        case gli::TARGET_CUBE_ARRAY:
            if(gli::is_compressed(Texture.format()))
                glCompressedTexSubImage3D(
                    Target, static_cast<GLint>(Level),
                    0, 0, 0,
                    Extent.x, Extent.y,
                    Texture.target() == gli::TARGET_3D ? Extent.z : LayerGL,
                    Format.Internal, static_cast<GLsizei>(Texture.size(Level)),
                    Texture.data(Layer, Face, Level));
            else
                glTexSubImage3D(
                    Target, static_cast<GLint>(Level),
                    0, 0, 0,
                    Extent.x, Extent.y,
                    Texture.target() == gli::TARGET_3D ? Extent.z : LayerGL,
                    Format.External, Format.Type,
                    Texture.data(Layer, Face, Level));
            break;
        default: assert(0); break;
        }
    }
    return TextureName;
} 
