#define GLEW_STATIC
#define GLM_ENABLE_EXPERIMENTAL
//#define GLM_FORCE_RADIANS
#include <iostream>
#include <fstream>
#include <exception>
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mouse.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <SOIL2.h>
#include "context.hpp"
#include "Material.hpp"

bool initSDL(SDL_Window *&window, SDL_GLContext &context);
GLuint CreateTexture(char const* Filename);

const std::string vertexPath = "resources/one.vert";
const std::string fragmentPath = "resources/one.frag";
const std::string diffusePath = "resources/fish.dds";
const std::string specularPath = "resources/fish-specular.dds";
const int windowWidth = 800;
const int windowHeight = 600;
const GLfloat cameraSpeed = 3.0f;

GLfloat vertices[] = {
        // Positions          // Normals           // Texture Coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
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
      std::cerr << "Debug context.\n";
    }

  GLuint vbo, vao, lightVao;
  glGenBuffers(1, &vbo);
  glGenVertexArrays(1, &vao);
  glGenVertexArrays(1, &lightVao);
  glBindVertexArray(vao);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  //position attrib
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
  glEnableVertexAttribArray(0);

  //surface normal attrib
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
  glEnableVertexAttribArray(1);

  //texcoord attrib
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
  glEnableVertexAttribArray(2);

  glBindVertexArray(0);

  Material shaderOne({vertexPath, fragmentPath});
  Material lampShader({vertexPath, "resources/lamp.frag"});

  //init texture
  GLuint texDiffuse = SOIL_load_OGL_texture(diffusePath.c_str(),
                                            SOIL_LOAD_AUTO,
                                            SOIL_CREATE_NEW_ID,
                                            SOIL_FLAG_DDS_LOAD_DIRECT | SOIL_FLAG_SRGB_COLOR_SPACE);
  if (texDiffuse == 0)
    std::cerr << "SOIL2 goof" << __LINE__;
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4.0f);
  glBindTexture(GL_TEXTURE_2D, 0);

  //init texture
  GLuint texSpecular = SOIL_load_OGL_texture(specularPath.c_str(),
                                             SOIL_LOAD_AUTO,
                                             SOIL_CREATE_NEW_ID,
                                             SOIL_FLAG_DDS_LOAD_DIRECT);
  if (texSpecular == 0)
    std::cerr << "SOIL2 goof" << __LINE__;
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4.0f);
  glBindTexture(GL_TEXTURE_2D, 0);

  //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glEnable(GL_MULTISAMPLE);
  glEnable(GL_DEPTH_TEST);
  //glEnable(GL_CULL_FACE);
  //glCullFace(GL_BACK);
  //glEnable(GL_BLEND);
  //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  //glEnable(GL_FRAMEBUFFER_SRGB);
  glClearColor(0.2, 0.2, 0.25, 1.0);


  shaderOne.SetUniform("diffuse", 0);
  shaderOne.SetUniform("specular", 1);

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

  std::vector<glm::vec3> lightPositions = {
    glm::vec3( 0.7f,  0.2f,  2.0f),
    glm::vec3( 2.3f, -3.3f, -4.0f),
    glm::vec3(-4.0f,  2.0f, -12.0f),
    glm::vec3( 0.0f,  0.0f, -3.0f)
  };

  glm::vec3 cameraPos(0.0f, 0.0f, 3.0f);
  glm::quat cameraQuat;

  SDL_Event windowEvent;

  unsigned int fpsLast = SDL_GetTicks();
  unsigned int fpsCurrent;
  unsigned int fpsFrames = 0;
  GLfloat deltaTime = 0.0f;
  GLfloat lastFrame = 0.0f;
  GLfloat fov = 45.0f;
  int mouseX, mouseY;
  bool nofocus = false;
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
        if (!nofocus) {
          mouseX = windowEvent.motion.xrel;
          mouseY = windowEvent.motion.yrel;
        }
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
      cameraPos.y -= cameraSpeed * deltaTime;
    if (keys[SDL_SCANCODE_E])
      cameraPos.y += cameraSpeed * deltaTime;
    if (keys[SDL_SCANCODE_Z]) {
      if (!nofocus) {
        nofocus = true;
        SDL_SetRelativeMouseMode(SDL_FALSE);
      }
      else {
        nofocus = false;
        SDL_SetRelativeMouseMode(SDL_TRUE);
      }
    }
    if (keys[SDL_SCANCODE_ESCAPE])
      break;

    glm::vec2 mouseDelta(mouseX, mouseY);
    glm::quat deltaPitch = glm::angleAxis(0.08f * mouseDelta.y * deltaTime, glm::vec3(1.0f, 0.0f, 0.0f));
    glm::quat deltaYaw = glm::angleAxis(0.08f * mouseDelta.x * deltaTime, glm::vec3(0.0f, 1.0f, 0.0f));
    mouseX = mouseY = 0;
    cameraQuat = deltaPitch * cameraQuat * deltaYaw;
    cameraQuat = glm::normalize(cameraQuat);
    glm::mat4 rotate = glm::mat4_cast(cameraQuat);
    glm::mat4 translate(1.0f);
    translate = glm::translate(translate, -cameraPos);
    glm::mat4 view = rotate * translate;

    for (unsigned int i = 0; i < lightPositions.size(); i++) {
      if (i % 2) {
        lightPositions[i].x = sin(time) / 2.0f;
      }
      else {
        lightPositions[i].y = sin(time) / 2.5f;
      }
    }

    //render
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shaderOne.Use();

    shaderOne.SetUniform("viewPos", cameraPos);

    for (int i = 0; i < lightPositions.size(); i++) {
      using namespace std;
      std::string p1 = "pointLights[";
      std::string p2 = to_string(i);
      shaderOne.SetUniform(p1 + p2 + string("].position"), lightPositions[i]);
      shaderOne.SetUniform(p1 + p2 + string("].ambient"), glm::vec3(0.1f));
      shaderOne.SetUniform(p1 + p2 + string("].diffuse"), glm::vec3(0.8f));
      shaderOne.SetUniform(p1 + p2 + string("].specular"), glm::vec3(0.8f));
      shaderOne.SetUniform(p1 + p2 + string("].constant"), 1.0f);
      shaderOne.SetUniform(p1 + p2 + string("].linear"), 0.9f);
      shaderOne.SetUniform(p1 + p2 + string("].quadratic"), 0.032f);
    }
    shaderOne.SetUniform("material.shininess", 128.0f);

    shaderOne.SetUniform("dirLight.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
    shaderOne.SetUniform("dirLight.ambient", glm::vec3(0.05f));
    shaderOne.SetUniform("dirLight.diffuse", glm::vec3(0.2f));
    shaderOne.SetUniform("dirLight.specular", glm::vec3(0.7f));

    glm::vec3 camFront = glm::eulerAngles(cameraQuat);
    float z = -(cos(camFront.x) * cos(camFront.y));
    float y = -(sin(camFront.x) * cos(camFront.y));
    float x = sin(camFront.y);
    shaderOne.SetUniform("spotLight.position", cameraPos);
    shaderOne.SetUniform("spotLight.direction", glm::vec3(x, y, z));
    shaderOne.SetUniform("spotLight.innerCutOff", glm::cos(glm::radians(12.5f)));
    shaderOne.SetUniform("spotLight.outerCutOff", glm::cos(glm::radians(17.5f)));
    shaderOne.SetUniform("spotLight.ambient", glm::vec3(0.1f));
    shaderOne.SetUniform("spotLight.diffuse", glm::vec3(0.7f, 0.7f, 0.5f));
    shaderOne.SetUniform("spotLight.specular", glm::vec3(0.7f));
    shaderOne.SetUniform("spotLight.constant", 1.0f);
    shaderOne.SetUniform("spotLight.linear", 0.09f);
    shaderOne.SetUniform("spotLight.quadratic", 0.032f);


    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texDiffuse);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texSpecular);

    glm::mat4 proj = glm::perspective(fov, (GLfloat)windowWidth / (GLfloat)windowHeight, 0.1f, 100.0f);
    shaderOne.SetUniform("view", view);
    shaderOne.SetUniform("proj", proj);
    glBindVertexArray(vao);
    for (int i = 0; i < cubePositions.size(); i++) {
      glm::mat4 model = glm::translate(glm::mat4(1.0f), cubePositions[i]);
      model = glm::rotate(model, 15.0f * i, glm::vec3(1.0f, 0.5f, 0.5f));
      shaderOne.SetUniform("model", model);
      glDrawArrays(GL_TRIANGLES, 0, 36);
  }

    lampShader.Use();
    for (unsigned int i = 0; i < lightPositions.size(); i++) {
      glm::mat4 model = glm::translate(glm::mat4(1.0f), lightPositions[i]);
      model = glm::scale(model, glm::vec3(0.2f));
      lampShader.SetUniform("model", model);
      lampShader.SetUniform("view", view);
      lampShader.SetUniform("proj", proj);
      glDrawArrays(GL_TRIANGLES, 0, 36);
    }
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
