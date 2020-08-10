#pragma once

//#include "Modules/Math/Quaternion.hpp"
//#include "Modules/Math/mat4.hpp"
//#include "Modules/Math/vec3.hpp"
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <math.h>
#include <glm/vec3.hpp>					 // glm::vec3
#include <glm/vec4.hpp>					 // glm::vec4
#include <glm/mat4x4.hpp>				 // glm::mat4
#include <glm/ext/matrix_transform.hpp>	 // glm::translate, glm::rotate, glm::scale
#include <glm/ext/matrix_clip_space.hpp> // glm::perspective
#include <glm/ext/scalar_constants.hpp>	 // glm::pi

namespace Soon
{
	class Camera3D
	{
	public:
		Camera3D(void)
		{
			m_Pos = glm::vec3(0.0f, 0.0f, 0.0f);

			mFov = glm::radians(45.0f);
			mNear = 0.05f;
			mFar = 1000.0f;

			mAspect = (float)1280.0f / 705;
			//_aspect = win._width / win._height;
		}

		glm::mat4 GetViewMatrix(void)
		{
			return (mView);
		}

		glm::mat4 GetProjectionMatrix(void)
		{
			glm::mat4 proj;

			proj = glm::perspective(mFov, mAspect, mNear, mFar);
			proj[1][1] *= -1;

			return (proj);
		}

		//void Translate(glm::vec3 translate)
		//{
		//	translate.z = -translate.z;
		//	mView = glm::translate(GetViewMatrix(), translate);
		//	m_Pos.x = mView[3][0];
		//	m_Pos.y = mView[3][1];
		//	m_Pos.z = mView[3][2];
		//}
		void Translate(glm::vec3 delta)
		{
			m_Pos += delta;
			UpdateViewMatrix();
		}

		//void Rotate(float angle, glm::vec3 axis)
		//{
		//	mView = glm::rotate(GetViewMatrix(), angle, axis);
		//}

		void Rotate(glm::vec3 delta)
		{
			mRotation += delta;
			UpdateViewMatrix();
		}

		void UpdateViewMatrix()
		{
			glm::mat4 rotM = glm::mat4(1.0f);
			glm::mat4 transM;

			rotM = glm::rotate(rotM, glm::radians(mRotation.x * -1.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			rotM = glm::rotate(rotM, glm::radians(mRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
			rotM = glm::rotate(rotM, glm::radians(mRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

			glm::vec3 translation = m_Pos;
			translation.y *= -1.0f;

			transM = glm::translate(glm::mat4(1.0f), translation);

			mView = rotM * transM;
		}

	public:
		glm::vec3 m_Pos;
		glm::mat4 mView;
		glm::mat4 mProj;
		glm::vec3 mRotation = glm::vec3();
		glm::vec4 mViewPos = glm::vec4();

		float mFov;
		float mNear;
		float mFar;
		float mAspect;

		void Move()
		{
			glm::vec3 camFront;
			camFront.x = -cos(glm::radians(mRotation.x)) * sin(glm::radians(mRotation.y));
			camFront.y = sin(glm::radians(mRotation.x));
			camFront.z = cos(glm::radians(mRotation.x)) * cos(glm::radians(mRotation.y));
			camFront = glm::normalize(camFront);

			float moveSpeed = 1;

			glm::vec3 dir;
			float rot = 0.0f;

			if (glfwGetKey(GraphicsInstance::GetInstance()->GetWindow(), GLFW_KEY_W))
				m_Pos += camFront * moveSpeed;
			if (glfwGetKey(GraphicsInstance::GetInstance()->GetWindow(), GLFW_KEY_S))
				m_Pos -= camFront * moveSpeed;
			if (glfwGetKey(GraphicsInstance::GetInstance()->GetWindow(), GLFW_KEY_A))
				m_Pos -= glm::normalize(glm::cross(camFront, glm::vec3(0.0f, 1.0f, 0.0f))) * moveSpeed;
			if (glfwGetKey(GraphicsInstance::GetInstance()->GetWindow(), GLFW_KEY_D))
				m_Pos += glm::normalize(glm::cross(camFront, glm::vec3(0.0f, 1.0f, 0.0f))) * moveSpeed;
			//if ( glfwGetKey(GraphicsInstance::GetInstance()->GetWindow(), GLFW_KEY_SPACE))
			//	dir.y += 1.0f;
			//if ( glfwGetKey(GraphicsInstance::GetInstance()->GetWindow(), GLFW_KEY_LEFT_CONTROL))
			//	dir.y -= 1.0f;
			if ( glfwGetKey(GraphicsInstance::GetInstance()->GetWindow(), GLFW_KEY_LEFT))
				Rotate(glm::vec3(0.0f, -1.0f, 0.0f));
			if ( glfwGetKey(GraphicsInstance::GetInstance()->GetWindow(), GLFW_KEY_RIGHT))
				Rotate(glm::vec3(0.0f, 1.0f, 0.0f));

			UpdateViewMatrix();
		}
	};
} // namespace Soon