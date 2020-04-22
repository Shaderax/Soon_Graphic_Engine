#pragma once

#include <math.h>

template< typename T = float >
class vec4
{
	public :
		vec4( void ) {};
		~vec4( void ) {};

		T x;
		T y;
		T z;
		T w;
		
		vec3(T mx, T my, T mz, T mw) : x(mx), y(my), z(mz), w(mw)
		{
			
		}

		vec4<T> operator+(vec4<T> const& b)
		{
			vec4 Result;

			Result.x = this->x + b.x;
			Result.y = this->y + b.y;
			Result.z = this->z + b.z;
			Result.w = this->w + b.w;

			return (Result);
		}

		vec4<T> operator-(vec4<T> const& b)
		{
			vec4 Result;

			Result.x = this->x - b.x;
			Result.y = this->y - b.y;
			Result.z = this->z - b.z;
			Result.w = this->w - b.w;

			return (Result);
		}

		vec4<T>& operator+=(vec4<T> const& b)
		{
			this->x += b.x;
			this->y += b.y;
			this->z += b.z;
			this->w += b.w;

			return (*this);
		}

		vec4<T>& operator-=(vec4<T> const& b)
		{
			this->x -= b.x;
			this->y -= b.y;
			this->z -= b.z;
			this->w -= b.w;

			return (*this);
		}

		vec4<T>& operator=(vec4<T> const& b)
		{
			this->x = b.x;
			this->y = b.y;
			this->z = b.z;
			this->w = b.w;

			return (*this);
		}
/*
		vec4<T> Cross(vec4<T> const& a)
		{
			vec4 Result;

			Result.x = this->y * a.z - this->z * a.y;
			Result.y = this->z * a.x - this->x * a.z;
			Result.z = this->x * a.y - this->y * a.x;

			return (Result);
		}
*/
		float Magnitude( void )
		{
			return (sqrt((x * x) + (y * y) + (z * z) + (w * w)));
		}

		void	Normalize( void )
		{
			float mag;

			mag = this->Magnitude();
			if (mag == 0.0f)
				return ;
			this->x /= mag;
			this->y /= mag;
			this->z /= mag;
			this->w /= mag;
		}

};
// Point : 1, Vec : 0
/*
void	vec4_mul_scal(t_vec4 vec, float i, t_vec4 ret)
{
	ret[0] = vec[0] * i;
	ret[1] = vec[1] * i;
	ret[2] = vec[2] * i;
	ret[3] = 0;
}
*/
