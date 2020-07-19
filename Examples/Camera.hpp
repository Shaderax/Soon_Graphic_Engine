#pragma once

#include "Modules/Math/Quaternion.hpp"
#include "Modules/Math/mat4.hpp"
#include "Modules/Math/vec3.hpp"
#include <math.h>

namespace Soon
{
	class Camera3D
	{
		public:
			Camera3D( void )
			{
				m_Pos = vec3<float>(0.0f, 0.0f, 0.0f);
				_target = vec3<float>(0.0f, 0.0f, 0.0f);
				_up = vec3<float>(0.0f, 1.0f, 0.0f);

				_fov = 45.0f;
				_near = 0.05f;
				_far = 100.0f;

				_aspect = 16 / 9;
				//_aspect = win._width / win._height;
			}

			mat4<float> GetViewMatrix( void )
			{
				mat4<float> view;

				view.elem[3][0] = -m_Pos.x;
				view.elem[3][1] = -m_Pos.y;
				view.elem[3][2] = m_Pos.z;

				return (view * m_Rotation.GetRotationMatrix());
			}

			mat4< float >	GetProjectionMatrix( void )
			{
				//_aspect = (float)win._width / (float)win._height;

				mat4< float > proj;

				float scale = 1 / tan(_fov * M_PI / 180 / 2);

				proj(0, 0) = scale / _aspect;
				proj(1, 1) = -scale;
				proj(2, 2) = _far / (_near - _far);
				proj(2, 3) = -1;
				proj(3, 2) = (_near * _far) / (_near - _far);
				proj(3, 3) = 0;

				return (proj);
			}

			void SetTarget(vec3<float> target)
			{
				_target = target;
			}

		public:
			Quaternion m_Rotation;
			vec3<float> m_Pos;
			vec3<float>	_target;
			vec3<float>	_up;

			float _fov;
			float _near;
			float _far;
			float _aspect;
	};
}