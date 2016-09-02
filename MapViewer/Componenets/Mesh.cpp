#include "Mesh.h"

#include <iostream>
#include <fstream>


#include "Material.h"
#include "Camera.h"

#include "gtc/matrix_transform.hpp"
#include "gtx/transform.hpp"

//#define POINT_MODE

namespace Rendering
{
	Mesh::Mesh()
	{

		m_position = glm::vec3(0.0f, 0.0f, 0.0f);
		m_scale = glm::vec3(1.0f, 1.0f, 1.0f);
		m_orientation = glm::quat(glm::vec3(0,0,0));
	}
	Mesh::~Mesh()
	{
		glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(1, &m_buffer);
		glDeleteBuffers(1, &m_indexBuffer);;
	}
	void Mesh::Draw()
	{
		using namespace glm;

		glBindVertexArray(vao);
		mat4 scale = glm::scale(mat4(1.0f), m_scale);
		mat4 rotation = glm::mat4_cast(m_orientation);
		mat4 translation = glm::translate(m_position);

		Camera::ModelToWorld_Matrix = translation * rotation * scale;

		//m_material->Activate();

		// put texture to slot 0  ||GL_TEXTURE0 - 0,  GL_TEXTURE1 - 1, GL_TEXTURE2 - 2....
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, m_tempTexture);
#ifdef POINT_MODE
		glDrawArrays(GL_POINTS, 0, m_vertexCount);
#else
		glDrawElements(
			GL_TRIANGLES,      // mode
			m_indexCount,
			GL_UNSIGNED_INT,   // type
			0          // element array buffer offset
			);
#endif

		glBindVertexArray(0);
	}


	void Mesh::LoadFromList(std::vector<Vertex> &i_vertices, std::vector<unsigned int>&i_indices)
	{
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		using namespace std;

		// Load vertices
		{
			m_vertexCount = i_vertices.size();
			Vertex *vertices = i_vertices.data();

			glGenBuffers(1, &m_buffer);
			glBindBuffer(GL_ARRAY_BUFFER, m_buffer);
			glBufferData(GL_ARRAY_BUFFER,
				m_vertexCount*sizeof(Vertex),
				&vertices[0],
				GL_STATIC_DRAW);

			// Specify 
			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glEnableVertexAttribArray(2);
			glEnableVertexAttribArray(3);

			//use layout to tell the shader set location in order
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void *>(offsetof(Vertex, position)));
			glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(Vertex), reinterpret_cast<void *>(offsetof(Vertex, uv)));
			glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(Vertex), reinterpret_cast<void *>(offsetof(Vertex, color)));
			glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void *>(offsetof(Vertex, normal)));
		}

		// Load indices
		{
			m_indexCount = i_indices.size();
			GLuint* indices = i_indices.data();

			// Generate a buffer for the indices
			glGenBuffers(1, &m_indexBuffer);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indexCount * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
			
		}

		glBindVertexArray(0);
		
	}



}