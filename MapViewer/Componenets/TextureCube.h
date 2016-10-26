#pragma once

#include "Texture.h"
namespace Rendering
{

	class TextureCube : public Texture
	{
	public:

		void create_cube_map(const char* front,
			const char* back,
			const char* top,
			const char* bottom,
			const char* left,
			const char* right,
			GLuint* tex_cube);

		bool Init(const char* front,
			const char* back,
			const char* top,
			const char* bottom,
			const char* left,
			const char* right);

		bool ShowDown();

		void Activate();

		TextureCube();
		~TextureCube();

	private:
		bool load_cube_map_side(GLuint texture, GLenum side_target, const char* file_name);
		GLuint m_textureID;
	};

}