#pragma once

#include <math.h>


template< typename T = float >
class mat4
{
	public :
		mat4( void )
		{
			elem[0] = elem[5] = elem[10] = elem[15] = 1.0f;
			elem[1] = elem[2] = elem[3] = elem[4] = elem[6] = elem[7] = elem[8] = elem[9] = elem[11] = elem[12] = elem[13] = elem[14] = 0.0f;
		}

		mat4( T xx, T xy, T xz, T yx, T yy, T yz, T zx, T zy, T zz)
		{
			elem[0] = xx;
			elem[4] = xy;
			elem[8] = xz;

			elem[1] = yx;
			elem[5] = yy;
			elem[9] = yz;

			elem[2] = zx;
			elem[6] = zy;
			elem[10] = zz;
		} 

		~mat4( void ) {};

		T elem[16];

		mat4( const mat4<T>& other)
		{
			*this = other;
		}

		T& operator()(int y, int x)
		{
			return (elem[x * 4 + y]);
		}

		mat4<T>& operator=(const mat4<T> & b)
		{
			for (uint32_t index = 0 ;index < 16 ; index++)
				elem[index] = b.elem[index];
			return (*this);
		}

/*
0  4  8  12
1  5  9  13
2  6  10 14
3  7  11 15
*/
		mat4<T> operator*(mat4<T> const& b)
		{
   			mat4<T> tmp( elem[0]*b.elem[0] + elem[1]*b.elem[3] + elem[2]*b.elem[6],
                             elem[0]*b.elem[1] + elem[1]*b.elem[4] + elem[2]*b.elem[7],
                             elem[0]*b.elem[2] + elem[1]*b.elem[5] + elem[2]*b.elem[8],
                             elem[3]*b.elem[0] + elem[4]*b.elem[3] + elem[5]*b.elem[6],
                             elem[3]*b.elem[1] + elem[4]*b.elem[4] + elem[5]*b.elem[7],
                             elem[3]*b.elem[2] + elem[4]*b.elem[5] + elem[5]*b.elem[8],
                             elem[6]*b.elem[0] + elem[7]*b.elem[3] + elem[8]*b.elem[6],
                             elem[6]*b.elem[1] + elem[7]*b.elem[4] + elem[8]*b.elem[7],
                             elem[6]*b.elem[2] + elem[7]*b.elem[5] + elem[8]*b.elem[8] );

			return this->operator=(tmp);
		}

		mat4<T>& operator*=(mat4<T> const& b)
		{
			return (*this) = (*this) * b;
		}
};
