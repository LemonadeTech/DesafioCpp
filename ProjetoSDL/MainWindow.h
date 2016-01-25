#ifndef INCLUDED_MAINWINDOW_H
#define INCLUDED_MAINWINDOW_H

#include <iostream>
#include <SDL2/SDL.h>
#undef main

#if defined(_WIN32)
    #pragma comment(lib, "Opengl32.lib")
    #pragma comment(lib, "glew32s.lib")
    #pragma comment(lib, "SDL2.lib")

    #define GLEW_STATIC
    #include <GL\glew.h>
#else
    #include <OpenGL/gl3.h>
    #include <OpenGL/gl3ext.h>
    #include <OpenGL/glu.h>
#endif

#include <mathfu/glsl_mappings.h>
using namespace mathfu;

#include <string>
#include <fstream>
using namespace std;

#include "tga.h"
#include "tiny_obj_loader.h"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

class MainWindow {

public:
   MainWindow();
   ~MainWindow();
   void render();

private:
   bool init();
   void initSDL();
   bool loadFromFile(const std::string &filename, GLuint &shader, int kindOfVertex);
   bool createProgram();
   void compileShader();
   void loadFighter(const std::string &inputfile);
   void loadTextures();
   bool canInitWindowAndContext();


private:

   bool alreadyInit;

   SDL_Window * sdlWindow;
   SDL_GLContext glContext;

   unsigned int windowWidth;
   unsigned int windowHeight;

   GLuint shader_program;
   GLuint shader_vertex_shader;
   GLuint shader_fragment_shader;

   GLint vertex_attribute;
   GLint texture_attribute;
   GLint normal_attribute;

   GLint color_uniform;
   GLint modelViewMatrix_uniform;
   GLint projectionMatrix_uniform;
   GLint textUnit0_uniform;

   GLint light0Position_uniform;
   GLint light0Color_uniform;

   GLuint texture0;

   GLuint sphere_vertex_array;
   GLuint sphere_indices_buffer;
   GLuint sphere_vertex_buffer;
   GLuint sphere_normals_buffer;
   GLuint sphere_texture_buffer;

   vec3 light0Pos;
   vec3 light0Color;

   std::vector<tinyobj::shape_t> shapes;
   std::vector<tinyobj::material_t> materials;

   int shapdeIx;
   float rotY;

   vec3 cameraPosition;
   vec3 cameraTarget;
};

#endif
