#ifndef MESH_H
#define MESH_H

#include <glew.h> // include GLEW and new version of GL on Windows
#include <glfw3.h> // GLFW helper library
#include <glm.hpp>
#include <vector>
namespace Rendering
{
	class MeshPoints
	{
	public:
		struct Vertex
		{
			GLfloat position[3];
			GLfloat color[3];
			Vertex()
			{}

		};
		GLuint          m_vao;

		GLint     m_object2World_location;
		glm::vec3 m_position;

		GLuint	m_buffer;
		GLuint  m_indexBuffer;

		unsigned int m_indexCount;
		unsigned int m_vertexCount;

		//This is pointCount, pointCount * 3 = floatCount
		void LoadFromPointList(void *i_pData, unsigned int pointCount);
		void LoadFromPointList(std::vector<Vertex> &i_list);

		void Draw();

		MeshPoints();
		~MeshPoints();
	};
}
#endif