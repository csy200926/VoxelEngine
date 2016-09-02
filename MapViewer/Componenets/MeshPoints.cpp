#include "MeshPoints.h"
#include <iostream>
#include <fstream>
namespace Rendering
{
	MeshPoints::MeshPoints()
	{

	}
	MeshPoints::~MeshPoints()
	{

	}
	void MeshPoints::Draw()
	{
		glBindVertexArray(m_vao);
		glDrawArrays(GL_POINTS, 0, m_vertexCount);
	}


	void MeshPoints::LoadFromPointList(std::vector<Vertex> &i_list)
	{
		LoadFromPointList(i_list.data(), i_list.size());
	}

	void MeshPoints::LoadFromPointList(void *i_pData, unsigned int count)
	{
		// Create vertex array object
		glGenVertexArrays(1, &m_vao);
		glBindVertexArray(m_vao);

		// Create vertex buffer object
		GLuint vbo;
		glGenBuffers(1, &vbo);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * count, i_pData, GL_STATIC_DRAW);

		//https://www.opengl.org/sdk/docs/man/html/glVertexAttribPointer.xhtml
		// Stride: array stride length, how long should it go each data grab
		// Pointer: offset of the first component of the first generic vertex attribute in the array
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void *>(offsetof(Vertex, position)));

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void *>(offsetof(Vertex, color)));

		m_vertexCount = count;
	}
}