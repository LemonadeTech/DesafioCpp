/*
 * Texture.h
 *
 *  Created on: Jan 18, 2016
 *      Author: fabiano
 */

#ifndef TEXTURE_H_
#define TEXTURE_H_

#include <string>
#include <GL/glew.h>

class Texture {
	public:

		Texture(const std::string &fileName);

		virtual ~Texture();

		void Bind(unsigned int unit);

	private:

		GLuint m_texture;
};

#endif /* TEXTURE_H_ */
