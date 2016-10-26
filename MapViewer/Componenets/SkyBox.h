#ifndef SKYBOX_H
#define SKYBOX_H

#include <glew.h> // include GLEW and new version of GL on Windows
#include <glfw3.h> // GLFW helper library
#include <glm.hpp>

namespace Rendering
{
	class Material;
	class Texture;
	class SkyBox
	{
	private:

		bool load_cube_map_side(GLuint texture, GLenum side_target, const char* file_name);
		GLuint make_big_cube();
		void create_cube_map(const char* front,
			const char* back,
			const char* top,
			const char* bottom,
			const char* left,
			const char* right,
			GLuint* tex_cube);


		Material *m_material;
		Texture *m_textureCube;
	public:
		GLuint  m_vao;
		GLuint	m_buffer;
		GLuint  m_indexBuffer;

		GLuint m_cubeMapTexture;

		void Draw();

		void Init(Texture *i_textureCube);
		void ShowDown();

		SkyBox();
		~SkyBox();
	};

}


#endif