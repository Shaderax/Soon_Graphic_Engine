#pragma once

#include <math.h>
#include <iostream>

template< typename T = float >
class vec3
{
	public :
		vec3( void ) : x(0), y(0), z(0) {};
		~vec3( void ) {};

		T x;
		T y;
		T z;
		
		vec3(T mx, T my, T mz) : x(mx), y(my), z(mz)
		{
			
		}
		
		vec3(vec3<T> const& v)
		{
			this->x = v.x;
			this->y = v.y;
			this->z = v.z;
		}
		
		vec3<T> operator*(float const& b)
		{
			vec3 Result;

			Result.x = this->x * b;
			Result.y = this->y * b;
			Result.z = this->z * b;

			return (Result);
		}

		vec3<T> operator+(vec3<T> const& b)
		{
			vec3 Result;

			Result.x = this->x + b.x;
			Result.y = this->y + b.y;
			Result.z = this->z + b.z;

			return (Result);
		}

		vec3<T> operator-(vec3<T> const& b)
		{
			vec3 Result;

			Result.x = this->x - b.x;
			Result.y = this->y - b.y;
			Result.z = this->z - b.z;

			return (Result);
		}

		vec3<T>& operator+=(vec3<T> const& b)
		{
			this->x += b.x;
			this->y += b.y;
			this->z += b.z;

			return (*this);
		}

		vec3<T>& operator-=(vec3<T> const& b)
		{
			this->x -= b.x;
			this->y -= b.y;
			this->z -= b.z;

			return (*this);
		}

		vec3<T>& operator=(vec3<T> const& b)
		{
			this->x = b.x;
			this->y = b.y;
			this->z = b.z;

			return (*this);
		}

		vec3<T> Cross(vec3<T> const& a)
		{
			vec3 Result;

			Result.x = this->y * a.z - this->z * a.y;
			Result.y = this->z * a.x - this->x * a.z;
			Result.z = this->x * a.y - this->y * a.x;

			return (Result);
		}

		float Magnitude( void )
		{
			return (sqrt((x * x) + (y * y) + (z * z)));
		}

		void	NormalizeVec( void )
		{
			float mag;

			mag = this->Magnitude();
			if (mag == 0.0f)
				return ;
			this->x /= mag;
			this->y /= mag;
			this->z /= mag;
		}
		
		vec3<T>	Normalize( void )
		{
			vec3<T> Result = *this;
			float mag;

			mag = Result.Magnitude();
			if (mag == 0.0f)
				return Result;
			Result.x /= mag;
			Result.y /= mag;
			Result.z /= mag;

			return (Result);
		}

		void show( void )
		{
			std::cout << x << " " << y << " " << z << std::endl;
		}
};
