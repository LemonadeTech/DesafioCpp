/*
 * Mesh.h
 *
 *  Created on: Jan 18, 2016
 *      Author: fabiano
 */

#ifndef MESH_H_
#define MESH_H_

#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <GL/glew.h>

#include "tinyobjloader/tiny_obj_loader.h"

class Vertex
{
	public:
		Vertex(const glm::vec3&pos)
		{
			this->pos = pos;
		}

	private:
		glm::vec3 pos;
};


class Mesh
{
	public:
		Mesh(const std::string&fileName);

		virtual ~Mesh();

		void Draw();

	private:

        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;

        enum {
        	POSITION_VB,
			NORMAL_VB,
			TEXTCOORD_VB,
			INDICE_VB,
			BUFFER_LIST_SIZE
        };

        GLuint m_vertexArrayObject;
        GLuint m_vertexArrayBufferList[BUFFER_LIST_SIZE];
        unsigned int m_drawCount;

};


#endif /* MESH_H_ */
