#ifndef __RENDER_H__
#define __RENDER_H__

#include <SDL2/SDL.h>

#include <memory>
#include <vector>

#include "tiny_obj_loader.h"
#include "mathfu/glsl_mappings.h"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

using namespace mathfu;

#if defined(_WIN32)
    #pragma comment(lib, "Opengl32.lib")
    #pragma comment(lib, "glew32s.lib")
    #pragma comment(lib, "SDL2.lib")

    #define GLEW_STATIC
    #include <GL\glew.h>
#elif defined(__linux__)
    #include <GL/glew.h>
    #include <GL/gl.h>
    #include <GL/glext.h>
    #include <GL/glu.h>
#else // OS X
    #include <OpenGL/gl3.h>
    #include <OpenGL/gl3ext.h>
    #include <OpenGL/glu.h>
#endif

class Render
{
public:
    Render(int width = 320, int height = 240, const std::string &title = "");
    ~Render();

    bool init();

    bool poll();
    void draw();

private:
    void compileShader(const std::string &shader, const std::string &frag);
    bool loadObj(const std::string &filename);
    void loadTextures(const std::string &filename);

    int m_windowWidth;
    int m_windowHeight;
    std::string m_windowTitle;

    int m_windowX;
    int m_windowY;

    std::shared_ptr<SDL_Window> m_sdlWindow;
    SDL_GLContext m_glContext;

    // NOTE: could have split on more classes, this is the reason I let
    // these variables with comments on where they are used

    // compileShader, draw
    GLuint m_shaderProgram;

    GLint m_vertexAttribute;
    GLint m_textureAttribute;
    GLint m_normalAttribute;

    GLint m_colorUniform;
    GLint m_modelViewMatrixUniform;
    GLint m_projectionMatrixUniform;
    GLint m_textUnit0Uniform;
    GLint m_light0ColorUniform;
    GLint m_light0PositionUniform;

    // loadTexture, draw
    GLuint m_texture0;

    // loadObj, draw
    // BUFFER_OFFSET...
    GLuint m_sphereVertexArray;
    std::vector<tinyobj::shape_t> m_shapes;
    std::vector<tinyobj::material_t> m_materials;
    int m_shapdeIx = 0;

    // draw
    vec3 m_cameraPosition;
    vec3 m_cameraTarget;
    float m_rotY = 0.0f;
    vec3 m_light0Pos;
    vec3 m_light0Color;
};

#endif // __RENDER_H__
