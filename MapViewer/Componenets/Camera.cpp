#include "Camera.h"
//#define GLM_DEPTH_ZERO_TO_ONE
#include "gtc/matrix_transform.hpp"
#include "gtx/quaternion.hpp"
#include <stdio.h>
namespace Rendering
{
	glm::mat4x4 Camera::ModelToWorld_Matrix;
	glm::mat4x4 Camera::WorldToView_Matrix;
	glm::mat4x4 Camera::Projective_Matrix;

	glm::vec3 Camera::cameraPos;
	glm::vec3 Camera::viewVector;
	glm::vec3 Camera::upVector(0, 1, 0);

	void Camera::SetLookAt(glm::vec3& i_eye, glm::vec3& i_target, glm::vec3& i_up)
	{
		viewVector = i_target;
		cameraPos = i_eye;
		WorldToView_Matrix = glm::lookAt(i_eye, i_target, i_up);

	}
	void Camera::SetPerspective(float i_fov, float i_aspect, float i_zNear, float i_zFar)
	{

		Projective_Matrix = glm::perspective(i_fov, i_aspect, i_zNear, i_zFar);
	}

	void Camera::Update()
	{
		//printf("cameraPos %f,%f,%f \n", cameraPos.x, cameraPos.y, cameraPos.z);
		//printf("viewVector %f,%f,%f \n", viewVector.x, viewVector.y, viewVector.z);
		WorldToView_Matrix = glm::lookAt(cameraPos, viewVector, glm::vec3(0, 1, 0));
	}



	glm::vec3 Camera::GetLeftDir()
	{
		return normalize(cross(normalize(viewVector - cameraPos), upVector));
	}
	glm::vec3 Camera::GetForwardDir()
	{
		return normalize(viewVector - cameraPos);
	}
	void Camera::RotateCamera(double Angle, double x, double y, double z)
	{
		using namespace glm;
		quat temp, quat_view, result;
		temp.x = x * sin(Angle * 0.5f);
		temp.y = y * sin(Angle * 0.5f);
		temp.z = z * sin(Angle * 0.5f);
		temp.w = cos(Angle * 0.5f);

		vec3 viewDir = normalize(viewVector - cameraPos);

		quat_view.x = viewDir.x;
		quat_view.y = viewDir.y;
		quat_view.z = viewDir.z;
		quat_view.w = 0;

		result = (temp * quat_view) * conjugate(temp);

		viewVector = cameraPos + vec3(result.x, result.y, result.z) * 2.0f;

	}
}