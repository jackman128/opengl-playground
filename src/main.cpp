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
#include "context.hpp"
#include "Shader.hpp"
#include "Model.hpp"

bool initSDL(SDL_Window *&window, SDL_GLContext &context);
GLuint CreateTexture(char const* Filename);

const std::string vertexPath = "resources/one.vert";
const std::string fragmentPath = "resources/one.frag";
const int windowWidth = 1920;
const int windowHeight = 1080;
const GLfloat cameraSpeed = 3.0f;


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

  Shader shaderOne({vertexPath, fragmentPath});
  Model myModel("resources/wt_teapot.obj");

  //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glEnable(GL_MULTISAMPLE);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  //glEnable(GL_BLEND);
  //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_FRAMEBUFFER_SRGB);
  glClearColor(0.2, 0.2, 0.25, 1.0);


  glm::vec3 cameraPos(0.0f, 0.0f, 3.0f);
  glm::quat cameraQuat;

  std::vector<glm::vec3> lightPositions = {
    glm::vec3( 0.7f,  0.2f,  2.0f),
    glm::vec3( 2.3f, -3.3f, -4.0f),
    glm::vec3(-4.0f,  2.0f, -12.0f),
    glm::vec3( 0.0f,  0.0f, -3.0f)
  };

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

    //update camera
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
    shaderOne.SetUniform("view", view);

    shaderOne.SetUniform("viewPos", cameraPos);

    /*
    for (int i = 0; i < lightPositions.size(); i++) {
      string p1 = "pointLights[";
      string p2 = to_string(i);
      shaderOne.SetUniform(p1 + p2 + string("].position"), lightPositions[i]);
      shaderOne.SetUniform(p1 + p2 + string("].ambient"), glm::vec3(0.2f));
      shaderOne.SetUniform(p1 + p2 + string("].diffuse"), glm::vec3(1.0f));
      shaderOne.SetUniform(p1 + p2 + string("].specular"), glm::vec3(1.2f));
      shaderOne.SetUniform(p1 + p2 + string("].constant"), 1.0f);
      shaderOne.SetUniform(p1 + p2 + string("].linear"), 0.9f);
      shaderOne.SetUniform(p1 + p2 + string("].quadratic"), 0.032f);
    }
    */

    shaderOne.SetUniform("dirLight.direction", glm::vec3(0.2f, 1.0f, 0.3f));

    glm::mat4 proj = glm::perspective(fov, (GLfloat)windowWidth / (GLfloat)windowHeight, 0.1f, 100.0f);
    shaderOne.SetUniform("proj", proj);

    glm::mat4 model;
    model = glm::translate(model, glm::vec3(sin(time)*1.5f, -1.75f, cos(time)*1.5f));
    model = glm::scale(model, glm::vec3(1.2f, 1.2f, 1.2f));
    shaderOne.SetUniform("model", model);

    //render
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shaderOne.Use();
    myModel.Draw(shaderOne);

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
  SDL_GL_DeleteContext(context);
  context = NULL;
  SDL_DestroyWindow(window);
  window = NULL;
  SDL_Quit();
  return 0;
}

bool initSDL(SDL_Window *&window, SDL_GLContext &context) {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cout << "SDL error: " << SDL_GetError() << std::endl;
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
    window = SDL_CreateWindow("OpenGL Playground", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              windowWidth, windowHeight, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
    if (window == NULL) {
      cout << "SDL error: failed to create window." << endl;
      return false;
    }
    context = SDL_GL_CreateContext(window);
    //enable VSync
    SDL_GL_SetSwapInterval(1);
    return true;
  }
}
