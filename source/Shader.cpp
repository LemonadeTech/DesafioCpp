/*
 * Shader.cpp
 *
 *  Created on: Jan 18, 2016
 *      Author: fabiano
 */

#include <iostream>
#include <fstream>

#include "Shader.h"


static std::string LoadShader(const std::string&fileName);
static void CheckShaderError(GLuint shader, GLuint flag, bool isProgram, const std::string&retErrorMessage);
static GLuint CreateShader(const std::string&text, GLenum shaderType);

/*################################################################################*/
Shader::Shader(const std::string&fileName)
{
	m_program = glCreateProgram();
	m_shaderList[0] = CreateShader(LoadShader(fileName+".vs"), GL_VERTEX_SHADER);
	m_shaderList[1] = CreateShader(LoadShader(fileName+".fs"), GL_FRAGMENT_SHADER);

	for(unsigned int i = 0; i < SHADERLIST_SIZE; i++) {
		glAttachShader(m_program, m_shaderList[i]);
	}

	glBindAttribLocation(m_program, 0, "position");
	glBindAttribLocation(m_program, 1, "normal");
	glBindAttribLocation(m_program, 2, "textCoord");

	glLinkProgram(m_program);
	CheckShaderError(m_program, GL_LINK_STATUS, true, "Error: Program linking failed: ");

	glValidateProgram(m_program);
	CheckShaderError(m_program, GL_VALIDATE_STATUS, true, "Error: Program is invalid: ");

	m_uniformList[TRANSFORM_U] = glGetUniformLocation(m_program, "transform");
}

/*################################################################################*/
Shader::~Shader()
{
	for(unsigned int i = 0; i < SHADERLIST_SIZE; i++) {
		glDetachShader(m_program, m_shaderList[i]);
		glDeleteShader(m_shaderList[i]);
	}
	glDeleteProgram(m_program);
}

/*################################################################################*/
void Shader::Bind()
{
	glUseProgram(m_program);
}

/*################################################################################*/
void Shader::Update(const Transform *transform, const Camera *camera)
{
	glm::mat4 model = camera->GetWorldToViewMatrix() * transform->GetModel();
	glUniformMatrix4fv(m_uniformList[TRANSFORM_U], 1, GL_FALSE, &model[0][0]);
}

/*################################################################################*/
static GLuint CreateShader(const std::string&text, GLenum shaderType)
{
	GLuint shader = glCreateShader(shaderType);
	if (shader == 0) {
		std::cerr << "Error: Shader creation failed!" << std::endl;
	}

	const GLchar*shaderSourceStrings[1];
	GLint shaderSourceStringLength[1];

	shaderSourceStrings[0] = text.c_str();
	shaderSourceStringLength[0] = text.length();

	glShaderSource(shader, 1, shaderSourceStrings, shaderSourceStringLength);
	glCompileShader(shader);
	CheckShaderError(shader, GL_COMPILE_STATUS, false, "Error: Shader compilation failed: ");

	return shader;
}


/*################################################################################*/
static std::string LoadShader(const std::string&fileName)
{
	std::string output;
	std::string line;
	std::ifstream file;

	file.open(fileName.c_str());
	if (file.is_open()) {
		while (file.good()) {
			getline(file, line);
			output.append(line+"\n");
		}
	} else {
		std::cerr << "Unable to lad the shader: " << fileName << std::endl;
	}
	return output;
}

/*################################################################################*/
static void CheckShaderError(GLuint shader, GLuint flag, bool isProgram, const std::string&retErrorMessage)
{
	GLint success = 0;
	GLchar errorMessage[1024] = {0};

	if (isProgram) {
		glGetProgramiv(shader, flag, &success);
	} else {
		glGetShaderiv(shader, flag, &success);
	}

	if (success == GL_FALSE) {
		if (isProgram) {
			glGetProgramInfoLog(shader, sizeof(errorMessage), NULL, errorMessage);
		} else {
			glGetShaderInfoLog(shader, sizeof(errorMessage), NULL, errorMessage);
		}
		std::cerr << retErrorMessage << ": '" << errorMessage << "'" << std::endl;
	}
}

