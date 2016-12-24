#define GLEW_STATIC
#include <iostream>
#include <fstream>
#include <exception>
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>
#include "shader.hpp"

bool initSDL(SDL_Window *&window, SDL_GLContext &context);

const std::string vertexPath = "shaders/one.vert";
const std::string fragmentPath = "shaders/one.frag";

GLfloat vertices[] = {
  //position          color
   0.8f,  0.5f, 0.0f,  1.0f, 0.0f, 0.0f,
   0.8f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,
   0.2f,  0.0f, 0.0f,  0.0f, 0.0f, 1.0f,
  -0.8f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,
  -0.8f,  0.5f, 0.0f,  0.0f, 1.0f, 0.0f,
  -0.2f,  0.0f, 0.0f,  0.0f, 0.0f, 1.0f
};

GLuint indices[] = {
  0, 1, 2, //first tri
  3, 4, 5  //second tri
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
  
  GLuint vbo, vao;
  glGenBuffers(1, &vbo);
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  //position attrib
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
  glEnableVertexAttribArray(0);

  //color attrib
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
  glEnableVertexAttribArray(1);

  glBindVertexArray(0);
  
  GLuint ebo;
  glGenBuffers(1, &ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  Shader shaderOne(vertexPath, fragmentPath);

  glEnableVertexAttribArray(0);
  glBindVertexArray(0);

  //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glEnable(GL_MULTISAMPLE);

  SDL_Event windowEvent;

  unsigned int fpsLast = SDL_GetTicks();
  unsigned int fpsCurrent;
  unsigned int fpsFrames = 0;

  while (1) {
    //events
    if (SDL_PollEvent(&windowEvent)) {
      if (windowEvent.type == SDL_QUIT) break;
    }

    //update
    GLfloat time = SDL_GetTicks() / 1000.0f;
    GLfloat xOffset = (sin(time) / 2);
    GLint vertexOffset = glGetUniformLocation(shaderOne.Program, "offset");
    glUniform3f(vertexOffset, xOffset, 0.0f, 0.0f);

    //render
    glClearColor(0.8, 0.8, 0.8, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    shaderOne.Use();
    glBindVertexArray(vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

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
  glBindVertexArray(0);
  
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
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
    window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              800, 600, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
    if (window == NULL) {
      std::cout << "window goof" << std::endl;
      return false;
    }
    context = SDL_GL_CreateContext(window);
    SDL_GL_SetSwapInterval(1);
    return true;
  }

}

