//
//  main.cpp
//  ProjetoSDL
//
//  Copyright Â© 2016 Lemonade. All rights reserved.
//

#include <iostream>

#include <sdl.h>

#undef main

#if defined(_WIN32)
#pragma comment(lib, "Opengl32.lib")
#pragma comment(lib, "glew32s.lib")
#pragma comment(lib, "SDL2.lib")

#define GLEW_STATIC
#include <GL\glew.h>
#else
#include <GL/gl3.h>
#include <GL/gl3ext.h>
#include <GL/glu.h>
#endif

#include <mathfu/glsl_mappings.h>
using namespace mathfu;

#include <string>
#include <fstream>
using namespace std;

#include "tga.h"
#include "tiny_obj_loader.h"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

static SDL_Window *sdlWindow;
static SDL_GLContext glContext;

static unsigned int windowWidth = 800;
static unsigned int windowHeight = 600;

static GLuint shader_program;
static GLuint shader_vertex_shader;
static GLuint shader_fragment_shader;

static GLint vertex_attribute;
static GLint texture_attribute;
static GLint normal_attribute;

static GLint color_uniform;
static GLint modelViewMatrix_uniform;
static GLint projectionMatrix_uniform;
static GLint textUnit0_uniform;

static GLint light0Position_uniform;
static GLint light0Color_uniform;

static GLuint texture0;

static GLuint sphere_vertex_array;
static GLuint sphere_indices_buffer;
static GLuint sphere_vertex_buffer;
static GLuint sphere_normals_buffer;
static GLuint sphere_texture_buffer;

static vec3 light0Pos = vec3(10.0f, 0.0f, 10.0f);
static vec3 light0Color = vec3(0.3f, 0.3f, 0.3f);

static std::vector<tinyobj::shape_t> shapes;
static std::vector<tinyobj::material_t> materials;

static int shapdeIx = 0;
static float rotY = 0.0f;

static vec3 cameraPosition(0.0, 0.0, 5.0);
static vec3 cameraTarget(0.0, 0.0, 0.0);

void compileShader(){

	//Carrega e compila o vertex shader
	ifstream ifsvsh("vertex.vsh");
	std::string vshader((std::istreambuf_iterator<char>(ifsvsh)), std::istreambuf_iterator<char>());
	const char *vshader_cstr = vshader.c_str();
	shader_vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(shader_vertex_shader, 1, &vshader_cstr, NULL);
	glCompileShader(shader_vertex_shader);

	//Verifica o status do vertex shader
	GLint compileStatus = GL_TRUE;
	glGetShaderiv(shader_vertex_shader, GL_COMPILE_STATUS, &compileStatus);

	GLchar log[500];
	memset(log, 0, sizeof(log));

	if (compileStatus == GL_FALSE) {
		glGetShaderInfoLog(shader_vertex_shader, sizeof(log), NULL, log);
		std::cout << "Vertex shader: " << log << std::endl;
		glDeleteShader(shader_vertex_shader);
		return;
	}


	//Carrega e compila o fragment shader
	ifstream ifsfsh("fragment.fsh");
	std::string fshader((std::istreambuf_iterator<char>(ifsfsh)), std::istreambuf_iterator<char>());
	const char *fshader_cstr = fshader.c_str();
	shader_fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(shader_fragment_shader, 1, &fshader_cstr, NULL);
	glCompileShader(shader_fragment_shader);


	//Verifica o status do fragment shader
	compileStatus = GL_TRUE;
	glGetShaderiv(shader_fragment_shader, GL_COMPILE_STATUS, &compileStatus);

	memset(log, 0, sizeof(log));

	if (compileStatus == GL_FALSE) {
		glGetShaderInfoLog(shader_fragment_shader, sizeof(log), NULL, log);
		std::cout << "Fragment shader: " << log << std::endl;
		glDeleteShader(shader_vertex_shader);
		glDeleteShader(shader_fragment_shader);
		return;
	}


	//Cria o programa de shader fazendo a vinculaÃ§Ã£o do vertex com o fragment
	shader_program = glCreateProgram();
	glAttachShader(shader_program, shader_vertex_shader);
	glAttachShader(shader_program, shader_fragment_shader);
	glLinkProgram(shader_program);


	//Verifica o status do programa shader
	GLint linkStatus = GL_TRUE;
	glGetProgramiv(shader_program, GL_LINK_STATUS, &linkStatus);
	if (linkStatus == GL_FALSE) {
		glGetProgramInfoLog(shader_program, sizeof(log), NULL, log);
		std::cout << log << std::endl;
		glDeleteShader(shader_vertex_shader);
		glDeleteShader(shader_fragment_shader);
		glDeleteProgram(shader_program);
		return;
	}


	//Obtem-se os atributos do vertex shader e fragment shader
	vertex_attribute = glGetAttribLocation(shader_program, "vertex");
	texture_attribute = glGetAttribLocation(shader_program, "textureCoord");
	normal_attribute = glGetAttribLocation(shader_program, "normal");

	color_uniform = glGetUniformLocation(shader_program, "color");

	//1Âº ERRO: NÃ£o estava presente o modelViewMatrix no vertex shader
	modelViewMatrix_uniform = glGetUniformLocation(shader_program, "modelViewMatrix");
	projectionMatrix_uniform = glGetUniformLocation(shader_program, "projectionMatrix");
	textUnit0_uniform = glGetUniformLocation(shader_program, "textUnit0");

	//2Âº ERRO: NÃ£o estava presente o light0Color no vertex shader
	light0Color_uniform = glGetUniformLocation(shader_program, "light0Color");
	light0Position_uniform = glGetUniformLocation(shader_program, "light0Position");
}

void loadTextures(){
	GLint texture0Width;
	GLint texture0Height;
	GLint texture0Components;
	GLenum texture0Format;

	GLbyte * texture0Pixels = gltReadTGABits("fighter.tga", &texture0Width, &texture0Height, &texture0Components, &texture0Format, NULL);

	glGenTextures(1, &texture0);
	glBindTexture(GL_TEXTURE_2D, texture0);

	glTexImage2D(GL_TEXTURE_2D, 0, texture0Components, texture0Width, texture0Height, 0, texture0Format, GL_UNSIGNED_BYTE, texture0Pixels);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

bool init(){
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0){
		std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return false;
	}

	
	//SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	//SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 16);
	

	sdlWindow = SDL_CreateWindow("Projeto SDL!", 100, 100, windowWidth, windowHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

	if (sdlWindow == nullptr){
		std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return false;
	}

	glContext = SDL_GL_CreateContext(sdlWindow);

	if (glContext == NULL){
		printf("OpenGL context could not be created! SDL Error: %s\n", SDL_GetError());
		return false;
	}


	std::cout << "OpenGL v" << glGetString(GL_VERSION) << " using " << glGetString(GL_RENDERER) << std::endl;

#ifdef _WIN32
	glewExperimental = GL_TRUE;
	if (GLEW_OK != glewInit())
	{
		return false;
	}
#endif

	SDL_GL_SetSwapInterval(1);

	compileShader();

	{
		std::string inputfile = "fighter.obj";

		std::string err = tinyobj::LoadObj(shapes, materials, inputfile.c_str());

		if (!err.empty()) {
			std::cerr << err << std::endl;
			exit(1);
		}

		std::cout << "# of shapes    : " << shapes.size() << std::endl;
		std::cout << "# of materials : " << materials.size() << std::endl;

		glGenVertexArrays(1, &sphere_vertex_array);
		glBindVertexArray(sphere_vertex_array);

		//Especifica aonde estÃ£o os dados da geometria
		glGenBuffers(1, &sphere_vertex_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, sphere_vertex_buffer);
		glBufferData(GL_ARRAY_BUFFER, shapes[shapdeIx].mesh.positions.size() * sizeof(float), &shapes[shapdeIx].mesh.positions[0], GL_STATIC_DRAW);
		glVertexAttribPointer(vertex_attribute, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, BUFFER_OFFSET(0));
		glEnableVertexAttribArray(vertex_attribute);

		//Especifica aonde estÃ£o os dados das normals
		glGenBuffers(1, &sphere_normals_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, sphere_normals_buffer);
		glBufferData(GL_ARRAY_BUFFER, shapes[shapdeIx].mesh.normals.size() * sizeof(float), &shapes[shapdeIx].mesh.normals[0], GL_STATIC_DRAW);
		glVertexAttribPointer(normal_attribute, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, BUFFER_OFFSET(0));
		glEnableVertexAttribArray(normal_attribute);

		//Especifica aonde estÃ£o os dados da textura
		glGenBuffers(1, &sphere_texture_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, sphere_texture_buffer);
		glBufferData(GL_ARRAY_BUFFER, shapes[shapdeIx].mesh.texcoords.size() * sizeof(float), &shapes[shapdeIx].mesh.texcoords[0], GL_STATIC_DRAW);
		glVertexAttribPointer(texture_attribute, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, BUFFER_OFFSET(0));
		glEnableVertexAttribArray(texture_attribute);


		//Especifica aonde estÃ£o os dados dos Ã­ndices
		glGenBuffers(1, &sphere_indices_buffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphere_indices_buffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, shapes[shapdeIx].mesh.indices.size() * sizeof(unsigned int), &shapes[shapdeIx].mesh.indices[0], GL_STATIC_DRAW);

		glBindVertexArray(0);
	}

	//glClearColor(.2, .2, .2, 1);
	glClearColor(0, 0, 0, 1);
	glEnable(GL_DEPTH_TEST);

	loadTextures();
	return true;
}

void render(){

	glClear(GL_COLOR_BUFFER_BIT);
	SDL_GL_SwapWindow(sdlWindow);

	glViewport(0, 0, windowWidth, windowHeight);

	//Ativa o programa shader
	glUseProgram(shader_program);

	mat4 projection = mat4::Perspective(.78f, ((float)windowWidth) / windowHeight, .1f, 1000.0f);
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

	rotY += 0.01f;
}

void mainLoop(){

	bool mainLoop = true;
	while (mainLoop){
		render();
	}
}

int main(int argc, const char * argv[]) {
	if (init())
		mainLoop();

	return 0;
}
