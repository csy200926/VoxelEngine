#include "Material.h"
#include <string>
#include <fstream>
#include <streambuf>
#include <stdlib.h>
#include <iostream>

#include "Texture.h"
#include "Camera.h"
namespace Rendering
{
	Material::Material()
	{
		m_texture = NULL;
		m_textureNormal = NULL;
	}


	Material::~Material()
	{
	}

	bool Material::Init(const char *i_vs, const char * i_fs)
	{

		// Load and compile the vertex and fragment shaders
		GLuint vertexShader = load_and_compile_shader(i_vs, GL_VERTEX_SHADER);
		GLuint fragmentShader = load_and_compile_shader(i_fs, GL_FRAGMENT_SHADER);

		// Attach the above shader to a program
		m_program = glCreateProgram();
		glAttachShader(m_program, vertexShader);
		glAttachShader(m_program, fragmentShader);

		// Flag the shaders for deletion
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		// Link and use the program
		glLinkProgram(m_program);
		glUseProgram(m_program);
		m_MMatrixLocation = glGetUniformLocation(m_program, "ModelToWorld_Matrix");
		m_PMatrixLocation = glGetUniformLocation(m_program, "Projective_Matrix");
		m_VMatrixLocation = glGetUniformLocation(m_program, "WorldToView_Matrix");
		


		return true;
	}

	bool Material::ShowDown()
	{
		if (m_program)
			glDeleteProgram(m_program);

		return true;
	}

	void Material::Activate()
	{
		glUseProgram(m_program);

		glUniformMatrix4fv(m_PMatrixLocation, 1, GL_FALSE, &Camera::Projective_Matrix[0][0]);
		glUniformMatrix4fv(m_VMatrixLocation, 1, GL_FALSE, &Camera::WorldToView_Matrix[0][0]);
		glUniformMatrix4fv(m_MMatrixLocation, 1, GL_FALSE, &Camera::ModelToWorld_Matrix[0][0]);

		if (m_texture != NULL)
			m_texture->Activate();

		if (m_textureNormal != NULL)
		{
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, m_textureNormal->m_textureID);
		}
	}

	void Material::SetTexture(Texture *i_texture)
	{
		m_texture = i_texture;

		// tell shader which uniform finds which slot
		GLint tex_loc = glGetUniformLocation(m_program, "basic_texture");
		glUniform1i(tex_loc, 0);
	}
	void Material::SetTextureNormal(Texture *i_texture)
	{
		m_textureNormal = i_texture;

		// tell shader which uniform finds which slot
		GLint tex_loc = glGetUniformLocation(m_program, "basic_texture");
		glUniform1i(tex_loc, 0);
	}


	GLuint Material::load_and_compile_shader(const char *fname, GLenum shaderType)
	{
		// Load a shader from an external file
		std::vector<char> buffer;
		read_shader_src(fname, buffer);
		const char *src = &buffer[0];

		// Compile the shader
		GLuint shader = glCreateShader(shaderType);
		glShaderSource(shader, 1, &src, NULL);
		glCompileShader(shader);
		// Check the result of the compilation
		GLint test;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &test);
		if (!test) {
			std::cerr << "Shader compilation failed with this message:" << std::endl;
			std::vector<char> compilation_log(512);
			glGetShaderInfoLog(shader, compilation_log.size(), NULL, &compilation_log[0]);
			std::cerr << &compilation_log[0] << std::endl;
			glfwTerminate();
			//exit(-1);
			system("pause");
		}
		return shader;

	}

	void Material::read_shader_src(const char *fname, std::vector<char> &buffer)
	{
		std::ifstream in;
		in.open(fname, std::ios::binary);

		if (in.is_open()) {
			// Get the number of bytes stored in this file
			in.seekg(0, std::ios::end);
			size_t length = (size_t)in.tellg();

			// Go to start of the file
			in.seekg(0, std::ios::beg);

			// Read the content of the file in a buffer
			buffer.resize(length + 1);
			in.read(&buffer[0], length);
			in.close();
			// Add a valid C - string end
			buffer[length] = '\0';
		}
		else {
			std::cerr << "Unable to open " << fname << " I'm out!" << std::endl;
			exit(-1);
		}
	}


}