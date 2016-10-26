#include "TextureCube.h"
#include <glew.h> // include GLEW and new version of GL on Windows
#include <glfw3.h> // GLFW helper library
#include <glm.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "../Utilities/stb_image.h"
namespace Rendering
{

	TextureCube::TextureCube()
	{
	}


	TextureCube::~TextureCube()
	{

	}

	bool TextureCube::load_cube_map_side(
		GLuint texture, GLenum side_target, const char* file_name
		) {
		glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

		int x, y, n;
		int force_channels = 4;
		unsigned char*  image_data = stbi_load(
			file_name, &x, &y, &n, force_channels);
		if (!image_data) {
			fprintf(stderr, "ERROR: could not load %s\n", file_name);
			return false;
		}
		// non-power-of-2 dimensions check
		if ((x & (x - 1)) != 0 || (y & (y - 1)) != 0) {
			fprintf(
				stderr, "WARNING: image %s is not power-of-2 dimensions\n", file_name
				);
		}

		// copy image data into 'target' side of cube map
		glTexImage2D(
			side_target,
			0,
			GL_RGBA,
			x,
			y,
			0,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			image_data
			);
		free(image_data);
		return true;
	}

	void TextureCube::create_cube_map(
		const char* front,
		const char* back,
		const char* top,
		const char* bottom,
		const char* left,
		const char* right,
		GLuint* tex_cube
		) {
		// generate a cube-map texture to hold all the sides
		glActiveTexture(GL_TEXTURE0);
		glGenTextures(1, tex_cube);

		// load each image and copy into a side of the cube-map texture
		(load_cube_map_side(*tex_cube, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, front));
		(load_cube_map_side(*tex_cube, GL_TEXTURE_CUBE_MAP_POSITIVE_Z, back));
		(load_cube_map_side(*tex_cube, GL_TEXTURE_CUBE_MAP_POSITIVE_Y, top));
		(load_cube_map_side(*tex_cube, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, bottom));
		(load_cube_map_side(*tex_cube, GL_TEXTURE_CUBE_MAP_NEGATIVE_X, left));
		(load_cube_map_side(*tex_cube, GL_TEXTURE_CUBE_MAP_POSITIVE_X, right));
		// format cube map texture
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}

	bool TextureCube::Init(const char* front,
		const char* back,
		const char* top,
		const char* bottom,
		const char* left,
		const char* right)
	{
		create_cube_map(front, back, top, bottom, left, right, &m_textureID);
		return true;
	}
	bool TextureCube::ShowDown()
	{
		return true;
	}

	void TextureCube::Activate()
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureID);

	}
}