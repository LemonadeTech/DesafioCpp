/*
 * Shader.h
 *
 *  Created on: Jan 18, 2016
 *      Author: fabiano
 */

#ifndef SHADER_H_
#define SHADER_H_

#include <string>
#include <GL/glew.h>

#include "Transform.h"
#include "Camera.h"

class Shader {
	public:

		Shader(const std::string&fileName);

		virtual ~Shader();

		void Bind();

		void Update(const Transform*transform, const Camera *camera);

	private:

		enum {
			TRANSFORM_U,

			UNIFORMS_SIZE
		};

		static const unsigned int SHADERLIST_SIZE = 2;
		GLuint m_program;
		GLuint m_shaderList[SHADERLIST_SIZE];
		GLuint m_uniformList[UNIFORMS_SIZE];
};

#endif /* SHADER_H_ */
