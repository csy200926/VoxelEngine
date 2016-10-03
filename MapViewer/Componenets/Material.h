#pragma once
#include <glew.h> // include GLEW and new version of GL on Windows
#include <glfw3.h> // GLFW helper library
#include <glm.hpp>
#include <vector>


namespace Rendering
{
	class Texture;
	class Material
	{
	private:

		Texture *m_texture;
		Texture *m_textureNormal;

		GLuint load_and_compile_shader(const char *fname, GLenum shaderType);
		void read_shader_src(const char *fname, std::vector<char> &buffer);

	public:
		GLint m_VMatrixLocation;
		GLint m_MMatrixLocation;
		GLint m_PMatrixLocation;

		// Hack
		GLint m_DepthMVPLocation;

		GLint GetProgram(){ return m_program; };

		bool Init(const char *i_vs, const char * i_fs);
		bool ShowDown();

		void SetTexture(Texture *i_texture);
		void SetTextureNormal(Texture *i_texture);

		Material();
		~Material();

		GLint m_program;

		void Activate();
	};


}

