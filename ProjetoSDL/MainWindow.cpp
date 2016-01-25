#include <iostream>
#include <string>
#include <fstream>

#include "MainWindow.h"

#include "tga.h"

using namespace std;

MainWindow::MainWindow(): alreadyInit(false)
{
   windowWidth = 800;
   windowHeight = 600;

   light0Pos = vec3(10.0f, 0.0f, 10.0f);
   light0Color = vec3(0.3f, 0.3f, 0.3f);

   shapdeIx = 0;
   rotY = 0.0f;

   cameraPosition = vec3(0.0, 0.0, 5.0);
   cameraTarget = vec3(0.0, 0.0, 0.0);
}

MainWindow::~MainWindow()
{
   SDL_GL_DeleteContext(glContext);
   SDL_DestroyWindow(sdlWindow);
   SDL_Quit();
}

void MainWindow::render()
{
   if(!alreadyInit)
      alreadyInit = init();

   //Se nao conseguiu forcar a inicializacao, sai fora!
   if(!alreadyInit)
      return;

   glClear(GL_COLOR_BUFFER_BIT);
   SDL_GL_SwapWindow(sdlWindow);

   glViewport(0, 0, windowWidth, windowHeight);

   glUseProgram(shader_program);


   mat4 projection = mat4::Perspective(.78f, ((float)windowWidth)/windowHeight, .1f, 1000.0f);
   mat4 view = mat4::LookAt(cameraTarget,
      cameraPosition,
      vec3(.0f, 1.0f, .0f));

   mat3 rotation = mat3::RotationY(rotY);
   mat4 model = mat4::FromTranslationVector(vec3(0.0, 0.0, 8.0)) * mat4::FromRotationMatrix(rotation) * mat4::FromRotationMatrix(mat3::RotationX(1.));
   mat4 modelViewMatrix = model * view;

   glUniformMatrix4fv(modelViewMatrix_uniform, 1, GL_FALSE, &modelViewMatrix[0]);
   glUniformMatrix4fv(projectionMatrix_uniform, 1, GL_FALSE, &projection[0]);

   glUniform4f(color_uniform, 1, 0, 0, 1);

   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, texture0);
   glUniform1i(textUnit0_uniform, 0);

   glUniform3f(light0Position_uniform, light0Pos.x(), light0Pos.y(), light0Pos.z());
   glUniform3f(light0Color_uniform, light0Color.x(), light0Color.y(), light0Color.z());

   glBindVertexArray(sphere_vertex_array);
   glDrawElements(GL_TRIANGLES, (GLsizei)shapes[shapdeIx].mesh.indices.size(), GL_UNSIGNED_INT, BUFFER_OFFSET(0));
   glBindVertexArray(0);

   glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr);

   rotY += 0.01f;
}

bool MainWindow::init()
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return false;
    }

    initSDL();

    if(!canInitWindowAndContext())
        return false;

#ifdef _WIN32
    glewExperimental = GL_TRUE;
    if (GLEW_OK != glewInit())
        return false;
#endif

    SDL_GL_SetSwapInterval(1);

    compileShader();

    loadFighter("fighter.obj");

    glEnable(GL_DEPTH_TEST);

    loadTextures();
    return true;
}

void MainWindow::initSDL()
{
   SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
   SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
   SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
   SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
   SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
   SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
   SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 16);
}

bool MainWindow::loadFromFile(const std::string &filename, GLuint &shader, int kindOfVertex)
{
    ifstream ifsvsh(filename);
    std::string vshader((std::istreambuf_iterator<char>(ifsvsh)), std::istreambuf_iterator<char>());
    const char *vshader_cstr = vshader.c_str();
    shader = glCreateShader(kindOfVertex);
    glShaderSource(shader, 1, &vshader_cstr, NULL);
    glCompileShader(shader);

    GLint compileStatus = GL_TRUE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);

    GLchar log[500];
    memset(log, 0, sizeof(log));

    if (compileStatus == GL_FALSE) {
        glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
        std::cout << filename <<  ": " << log << std::endl;
        glDeleteShader(shader);
        return false;
    }
    return true;
}

bool MainWindow::createProgram()
{
    shader_program = glCreateProgram();
    glAttachShader(shader_program, shader_vertex_shader);
    glAttachShader(shader_program, shader_fragment_shader);

    glLinkProgram(shader_program);

    GLchar log[500];
    GLint linkStatus = GL_TRUE;
    glGetProgramiv(shader_program, GL_LINK_STATUS, &linkStatus);
    if (linkStatus == GL_FALSE) {
        glGetProgramInfoLog(shader_program, sizeof(log), nullptr, log);
        std::cout << log << std::endl;
        glDeleteShader(shader_vertex_shader);
        glDeleteShader(shader_fragment_shader);
        glDeleteProgram(shader_program);
        return false;
    }

    return true;
}

void MainWindow::compileShader()
{
    if(!loadFromFile("vertex.vsh", shader_vertex_shader, GL_VERTEX_SHADER))
        return;

    if(!loadFromFile("fragment.fsh", shader_fragment_shader, GL_FRAGMENT_SHADER))
        return;

    if(!createProgram())
        return;


    vertex_attribute = glGetAttribLocation(shader_program, "vertex");
    texture_attribute = glGetAttribLocation(shader_program, "textureCoord");
    normal_attribute = glGetAttribLocation(shader_program, "normal");

    color_uniform = glGetUniformLocation(shader_program, "color");
    modelViewMatrix_uniform = glGetUniformLocation(shader_program, "modelViewMatrix");
    projectionMatrix_uniform = glGetUniformLocation(shader_program, "projectionMatrix");
    textUnit0_uniform = glGetUniformLocation(shader_program, "textUnit0");

    light0Color_uniform = glGetUniformLocation(shader_program, "light0Color");
    light0Position_uniform = glGetUniformLocation(shader_program, "light0Position");
}

void MainWindow::loadFighter(const std::string &inputfile)
{
    std::string err = tinyobj::LoadObj(shapes, materials, inputfile.c_str());

    if (!err.empty()) {
        std::cerr << err << std::endl;
        exit(1);
    }

    std::cout << "# of shapes    : " << shapes.size() << std::endl;
    std::cout << "# of materials : " << materials.size() << std::endl;

    glGenVertexArrays(1, &sphere_vertex_array);
    glBindVertexArray(sphere_vertex_array);

    glGenBuffers(1, &sphere_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, sphere_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, shapes[shapdeIx].mesh.positions.size() * sizeof(float), &shapes[shapdeIx].mesh.positions[0], GL_STATIC_DRAW);
    glVertexAttribPointer(vertex_attribute, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(vertex_attribute);


    glGenBuffers(1, &sphere_normals_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, sphere_normals_buffer);
    glBufferData(GL_ARRAY_BUFFER, shapes[shapdeIx].mesh.normals.size() * sizeof(float), &shapes[shapdeIx].mesh.normals[0], GL_STATIC_DRAW);
    glVertexAttribPointer(normal_attribute, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(normal_attribute);


    glGenBuffers(1, &sphere_texture_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, sphere_texture_buffer);
    glBufferData(GL_ARRAY_BUFFER, shapes[shapdeIx].mesh.texcoords.size() * sizeof(float), &shapes[shapdeIx].mesh.texcoords[0], GL_STATIC_DRAW);
    glVertexAttribPointer(texture_attribute, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(texture_attribute);

    glGenBuffers(1, &sphere_indices_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphere_indices_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, shapes[shapdeIx].mesh.indices.size() * sizeof(unsigned int), &shapes[shapdeIx].mesh.indices[0], GL_STATIC_DRAW);

    glBindVertexArray(0);

}

void MainWindow::loadTextures()
{
    GLint texture0Width;
    GLint texture0Height;
    GLint texture0Components;
    GLenum texture0Format;

    GLbyte * texture0Pixels = gltReadTGABits("fighter.tga", &texture0Width, &texture0Height, &texture0Components, &texture0Format, nullptr);

    glGenTextures(1, &texture0);
    glBindTexture(GL_TEXTURE_2D, texture0);

    glTexImage2D(GL_TEXTURE_2D, 0, texture0Components, texture0Width, texture0Height, 0, texture0Format, GL_UNSIGNED_BYTE, texture0Pixels);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

bool MainWindow::canInitWindowAndContext()
{
   sdlWindow = SDL_CreateWindow("Projeto SDL!", 600, 100, windowWidth, windowHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (sdlWindow == nullptr)
    {
        std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return false;
    }

    glContext = SDL_GL_CreateContext(sdlWindow);

    if( glContext == NULL)
    {
        printf( "SDL_CreateWindow Error: %s\n", SDL_GetError() );
        return false;
    }

    std::cout << "OpenGL v" << glGetString(GL_VERSION) << " using " << glGetString(GL_RENDERER) << std::endl;

    return true;
}


