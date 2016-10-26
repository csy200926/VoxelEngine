#include "SkyBox.h"

#include "Material.h"

namespace Rendering
{

	SkyBox::SkyBox()
	{
	}


	SkyBox::~SkyBox()
	{
		delete m_material;
	}

	GLuint SkyBox::make_big_cube() {
		float points[] = {
			-10.0f, 10.0f, -10.0f,
			-10.0f, -10.0f, -10.0f,
			10.0f, -10.0f, -10.0f,
			10.0f, -10.0f, -10.0f,
			10.0f, 10.0f, -10.0f,
			-10.0f, 10.0f, -10.0f,

			-10.0f, -10.0f, 10.0f,
			-10.0f, -10.0f, -10.0f,
			-10.0f, 10.0f, -10.0f,
			-10.0f, 10.0f, -10.0f,
			-10.0f, 10.0f, 10.0f,
			-10.0f, -10.0f, 10.0f,

			10.0f, -10.0f, -10.0f,
			10.0f, -10.0f, 10.0f,
			10.0f, 10.0f, 10.0f,
			10.0f, 10.0f, 10.0f,
			10.0f, 10.0f, -10.0f,
			10.0f, -10.0f, -10.0f,

			-10.0f, -10.0f, 10.0f,
			-10.0f, 10.0f, 10.0f,
			10.0f, 10.0f, 10.0f,
			10.0f, 10.0f, 10.0f,
			10.0f, -10.0f, 10.0f,
			-10.0f, -10.0f, 10.0f,

			-10.0f, 10.0f, -10.0f,
			10.0f, 10.0f, -10.0f,
			10.0f, 10.0f, 10.0f,
			10.0f, 10.0f, 10.0f,
			-10.0f, 10.0f, 10.0f,
			-10.0f, 10.0f, -10.0f,

			-10.0f, -10.0f, -10.0f,
			-10.0f, -10.0f, 10.0f,
			10.0f, -10.0f, -10.0f,
			10.0f, -10.0f, -10.0f,
			-10.0f, -10.0f, 10.0f,
			10.0f, -10.0f, 10.0f
		};
		GLuint vbo;
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(
		GL_ARRAY_BUFFER, 3 * 36 * sizeof(GLfloat), &points, GL_STATIC_DRAW);

		GLuint vao;
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		return vao;
	}


	void SkyBox::Draw()
	{
		glDepthMask(GL_FALSE);

		m_material->Activate();

		glBindVertexArray(m_vao);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glDepthMask(GL_TRUE);

	}

	void SkyBox::Init(Texture *i_textureCube)
	{

		m_material = new Material();
		m_material->Init("Shaders/cube_vs.shader", "Shaders/cube_fs.shader");
		m_material->SetTexture(i_textureCube);

		m_vao = make_big_cube();


	}

	void SkyBox::ShowDown()
	{

	}

}