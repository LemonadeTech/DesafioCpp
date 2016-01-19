/*
 * Texture.cpp
 *
 *  Created on: Jan 18, 2016
 *      Author: fabiano
 */

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "Texture.h"
#include "tga.h"

/*################################################################################*/
Texture::Texture(const std::string &fileName)
{
	GLint width;
	GLint height;
	GLint numComponents;
	GLenum format;

	GLbyte*texturePixels = gltReadTGABits(fileName.c_str(), &width, &height, &numComponents, &format, NULL);
	if(texturePixels == NULL) {
		std::cerr << "Texture loading failed for texture: " << fileName << std::endl;
	}

	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, numComponents, width, height, 0, format, GL_UNSIGNED_BYTE, texturePixels);

	free(texturePixels);
}

/*################################################################################*/
Texture::~Texture()
{
	glDeleteTextures(1, &m_texture);
}

/*################################################################################*/
void Texture::Bind(unsigned int unit)
{
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, m_texture);
}

