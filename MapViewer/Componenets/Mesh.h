#pragma once

#include <glew.h> // include GLEW and new version of GL on Windows
#include <glfw3.h> // GLFW helper library
#include <glm.hpp>
#include <gtc/quaternion.hpp>
#include <gtx/quaternion.hpp>
#include <vector>
#include "../Utilities/Debugging.h"
namespace Rendering
{
	struct Vertex
	{

		GLfloat position[3];
		GLfloat normal[3];
		GLubyte uv[4];// This UV is for voxel texture mapping purpose
		GLubyte color[4];
		//GLfloat tangent[3];

		Vertex()
		{}
		Vertex(GLfloat x, GLfloat y, GLfloat z)
		{
			position[0] = x;
			position[1] = y;
			position[2] = z;
			normal[0] = 0.0f;
			normal[1] = 0.0f;
			normal[2] = 0.0f;
			color[0] = 1.0f;
			color[1] = 1.0f;
			color[2] = 1.0f;
			color[3] = 1.0f;
			uv[0] = 1;
			uv[1] = 1;
			uv[2] = 1;
			uv[3] = 1;
		}
		void Set(glm::vec3 &i_position, glm::vec3& i_normal)
		{
			position[0] = i_position.x;
			position[1] = i_position.y;
			position[2] = i_position.z;
			normal[0] = i_normal.x;
			normal[1] = i_normal.y;
			normal[2] = i_normal.z;
			color[0] = 1.0f;
			color[1] = 1.0f;
			color[2] = 1.0f;
			color[3] = 1.0f;
			uv[0] = 1;
			uv[1] = 1;
			uv[2] = 1;
			uv[3] = 1;
		}
		void Set(glm::vec3 &i_position, glm::vec3& i_normal, glm::vec4 &i_uv)
		{
			position[0] = i_position.x;
			position[1] = i_position.y;
			position[2] = i_position.z;
			normal[0] = i_normal.x;
			normal[1] = i_normal.y;
			normal[2] = i_normal.z;
			color[0] = 1.0f;
			color[1] = 1.0f;
			color[2] = 1.0f;
			color[3] = 1.0f;
			uv[0] = i_uv.x;
			uv[1] = i_uv.y;
			uv[2] = i_uv.z;
			uv[3] = i_uv.w;
		}
		void SetColor(glm::vec3 &i_color)
		{
			color[0] = i_color[0];
			color[1] = i_color[1];
			color[2] = i_color[2];
			color[3] = 1.0f;
		}
		//Vertex(	GLfloat x, GLfloat y, GLfloat z, 
		//		GLfloat r, GLfloat g, GLfloat b, GLfloat a, 
		//		GLfloat nx, GLfloat ny, GLfloat nz,
		//		GLfloat u,GLfloat v)
		//{
		//	position[0] = x;
		//	position[1] = y;
		//	position[2] = z;
		//	normal[0] = nx;
		//	normal[1] = ny;
		//	normal[2] = nz;
		//	color[0] = r;
		//	color[1] = g;
		//	color[2] = b;
		//	color[3] = a;
		//	uv[0] = u;
		//	uv[1] = v;
		//}
	};

	class Material;

	class Mesh
	{
	public:
		

		GLuint vao;

		glm::vec3 m_position;
		glm::vec3 m_scale;
		glm::quat m_orientation;


		void LoadFromList(std::vector<Vertex> &i_vertices, std::vector<unsigned int>&i_indices);
		void Draw();


		Mesh();
		~Mesh();
	private:
		Material *m_material;

		GLuint	m_buffer;
		GLuint  m_indexBuffer;

		unsigned int m_indexCount;
		unsigned int m_vertexCount;
	};

}
