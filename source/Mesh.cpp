/*
 * Mesh.cpp
 *
 *  Created on: Jan 18, 2016
 *      Author: fabiano
 */
#include <stdio.h>
#include <iostream>

#include "Mesh.h"



/*################################################################################*/
Mesh::Mesh(const std::string&fileName)
{
	// Load file
	std::string err = tinyobj::LoadObj(shapes, materials, fileName.c_str());
	if (!err.empty()) {
	  std::cerr << err << std::endl;
	  exit(1);
	}
	std::cout << "# of shapes    : " << shapes.size() << std::endl;
	std::cout << "# of materials : " << materials.size() << std::endl;

	for (size_t i = 0; i < shapes.size(); i++) {
		printf("shape[%ld].name = %s\n", i, shapes[i].name.c_str());
		printf("Size of shape[%ld].mesh.positions.size(): %ld\n", i, shapes[i].mesh.positions.size());
		printf("Size of shape[%ld].mesh.normals.size(): %ld\n", i, shapes[i].mesh.normals.size());
		printf("Size of shape[%ld].indices: %ld\n", i, shapes[i].mesh.indices.size());
		printf("Size of shape[%ld].material_ids: %ld\n", i, shapes[i].mesh.material_ids.size());
	}


	glGenVertexArrays(1, &m_vertexArrayObject);
	glBindVertexArray(m_vertexArrayObject);

	// Generate buffer list
	glGenBuffers(BUFFER_LIST_SIZE, m_vertexArrayBufferList);

	// Add vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexArrayBufferList[POSITION_VB]);
	glBufferData(GL_ARRAY_BUFFER, shapes[0].mesh.positions.size() * sizeof(float), &shapes[0].mesh.positions[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);

	// Add normal buffer
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexArrayBufferList[NORMAL_VB]);
	glBufferData(GL_ARRAY_BUFFER, shapes[0].mesh.normals.size() * sizeof(float), &shapes[0].mesh.normals[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);

	// Add texture coordinates buffer
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexArrayBufferList[TEXTCOORD_VB]);
	glBufferData(GL_ARRAY_BUFFER, shapes[0].mesh.texcoords.size() * sizeof(float), &shapes[0].mesh.texcoords[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

	// Add vertex index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vertexArrayBufferList[INDICE_VB]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, shapes[0].mesh.indices.size() * sizeof(unsigned int), &shapes[0].mesh.indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);

	m_drawCount = shapes[0].mesh.positions.size();
}

/*################################################################################*/
Mesh::~Mesh()
{
	glDeleteVertexArrays(1, &m_vertexArrayObject);
}

/*################################################################################*/
void Mesh::Draw()
{
    glBindVertexArray(m_vertexArrayObject);
    glDrawElements(GL_TRIANGLES, (GLsizei)shapes[0].mesh.indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}



