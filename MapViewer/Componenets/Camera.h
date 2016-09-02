#pragma once
#include <glm.hpp>


namespace Rendering
{
	class Camera
	{
	public:
		static glm::mat4x4 ModelToWorld_Matrix;
		static glm::mat4x4 WorldToView_Matrix;
		static glm::mat4x4 Projective_Matrix;

		static glm::vec3 cameraPos;
		static glm::vec3 viewVector;
		static glm::vec3 upVector;

		static void SetLookAt(glm::vec3& i_eye, glm::vec3& i_target, glm::vec3& i_up);
		static void SetPerspective(float i_fovy, float i_aspect, float i_zNear, float i_zFar);
		static void RotateCamera(double Angle, double x, double y, double z);

		static glm::vec3 GetLeftDir();
		static glm::vec3 GetForwardDir();

		static void Update();
	protected:
	private:


	};

}

