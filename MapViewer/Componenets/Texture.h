#pragma once

#include <glew.h> // include GLEW and new version of GL on Windows
#include <glfw3.h> // GLFW helper library
#include <glm.hpp>

namespace Rendering
{


	class Texture
	{
	private:


	public:
		GLuint m_textureID;

		Texture();
		~Texture();

		bool virtual LoadFromPath(const char *i_path);
		bool virtual ShowDown();

		void virtual Activate();

	};




}
