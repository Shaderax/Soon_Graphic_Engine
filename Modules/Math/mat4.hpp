#pragma once

#include <math.h>

template< typename T = float >
class mat4
{
	public :
		mat4( void )
		{
			int x;
			int y;

			y = -1;
			while (++y < 4)
			{
				x = -1;
				while (++x < 4)
					this->elem[y][x] = (((y * 4) + x) % 5 == 0 ? 1.0f : 0.0f);
			}
		};

		~mat4( void ) {};

		T elem[4][4];

		mat4( const mat4<T>& other)
		{
			*this = other;
		}

		T& operator()(int y, int x)
		{
			return (elem[y][x]);
		}

		mat4<T>& operator=(const mat4<T> & b)
		{
			int x;
			int y;

			y = -1;
			while (++y < 4)
			{
				x = -1;
				while (++x < 4)
					this->elem[y][x] = b.elem[y][x];
			}
			return (*this);
		}

		mat4<T> operator*(mat4<T> const& b)
		{
			mat4<T> Result;
			int x;
			int y;

			y = -1;
			while (++y < 4)
			{
				x = -1;
				while (++x < 4)
					Result(y,x) = this->elem[y][0] * b.elem[0][x] + this->elem[y][1] * b.elem[1][x] +
						this->elem[y][2] * b.elem[2][x] + this->elem[y][3] * b.elem[3][x];
			}
			return (Result);
		}

		mat4<T>& operator*=(mat4<T> const& b)
		{
			return (*this) = (*this) * b;
		}

		mat4<T> operator-(mat4<T> const& b)
		{
			mat4<T> Result;
			int x;
			int y;

			y = -1;
			while (++y < 4)
			{
				x = -1;
				while (++x < 4)
					Result(y,x) = this(y,x) - b(y,x);
			}
			return (Result);
		}

		mat4<T>& operator-=(mat4<T> const& b)
		{
			return (*this) = (*this) - b;
		}

		mat4<T> operator+(mat4<T> const& b)
		{
			mat4<T> Result;
			int x;
			int y;

			y = -1;
			while (++y < 4)
			{
				x = -1;
				while (++x < 4)
					Result(y,x) = this(y,x) + b(y,x);
			}
			return Result;
		}

		mat4<T>& operator+=(mat4<T> const& b)
		{
			return (*this) = (*this) + b;
		}


		mat4	NewMat4RotateX(float angle)
		{
			mat4 mat2;

			mat2(1,1) = cos(angle);
			mat2(1,2) = -sin(angle);
			mat2(2,1) = sin(angle);
			mat2(2,2) = cos(angle);

			return (mat2);
		}

		mat4	NewMat4RotateY(float angle)
		{
			mat4 mat2;

			mat2(0,0) = cos(angle);
			mat2(0,2) = sin(angle);
			mat2(2,0) = -sin(angle);
			mat2(2,2) = cos(angle);

			return (mat2);
		}

		mat4	NewMat4RotateZ(float angle)
		{
			mat4 mat2;

			mat2(0,0) = cos(angle);
			mat2(0,1) = -sin(angle);
			mat2(1,0) = sin(angle);
			mat2(1,1) = cos(angle);

			return (mat2);
		}

		void	Rotate(float angle, int axis)
		{
			float	to_degres;

			to_degres = angle * (180.0f / M_PI);
			if (axis == 1)
				this = this * NewMat4RotateX(to_degres);
			else if (axis == 2)
				this = this * NewMat4RotateY(to_degres);
			else if (axis == 3)
				this = this * NewMat4RotateZ(to_degres);
		}
};
