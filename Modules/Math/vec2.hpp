#pragma once

#include <math.h>
#include <iostream>

template< typename T = float >
class vec2
{
	public :
		vec2( void ) {};
		~vec2( void ) {};

		T x;
		T y;

		vec2(T mx, T my) : x(mx), y(my)
		{
			
		}

		vec2<T> operator+(vec2<T> const& b)
		{
			vec2 Result;

			Result.x = this->x + b.x;
			Result.y = this->y + b.y;

			return (Result);
		}

		vec2<T> operator-(vec2<T> const& b)
		{
			vec2 Result;

			Result.x = this->x - b.x;
			Result.y = this->y - b.y;

			return (Result);
		}

		vec2<T>& operator+=(vec2<T> const& b)
		{
			this->x += b.x;
			this->y += b.y;

			return (*this);
		}

		vec2<T>& operator-=(vec2<T> const& b)
		{
			this->x -= b.x;
			this->y -= b.y;

			return (*this);
		}

		vec2<T>& operator=(vec2<T> const& b)
		{
			this->x = b.x;
			this->y = b.y;

			return (*this);
		}

		float Magnitude( void )
		{
			return (sqrt((x * x) + (y * y)));
		}

		void	Normalize( void )
		{
			float mag;

			mag = this->Magnitude();
			if (mag == 0.0f)
				return ;
			this->x /= mag;
			this->y /= mag;
		}

		void show( void )
		{
			std::cout << x << " " << y << std::endl;
		}

};
