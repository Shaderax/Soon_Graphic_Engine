#pragma once

#include "vec3.hpp"
#include "vec4.hpp"
#include "mat4.hpp"
#include <math.h>

struct Quaternion
{
	Quaternion( void )
	{
		v = vec4<float>(0.0f, 0.0f, 0.0f, 1.0f);
	}

	Quaternion(float x, float y, float z, float w)
	{
		v.x = x;
		v.y = y;
		v.z = z;
		v.w = w;
	}

	Quaternion( vec3<float> axis, float angle)
	{
		//axis is a unit vector
		angle = angle * M_PI / 180.0f;
		axis.NormalizeVec();
		v.x = axis.x * sin( angle / 2 );
		v.y = axis.y * sin( angle / 2 );
		v.z = axis.z * sin( angle / 2 );
		v.w = cos( angle / 2 );
	}

	~Quaternion( void ) {};

	mat4< float > GetRotationMatrix( void )
	{
		mat4<float> mat;

		mat(0, 0) = 1 - 2 * v.y * v.y - 2 * v.z * v.z;
		mat(0, 1) = 2 * v.x * v.y - v.w * v.z;
		mat(0, 2) = 2 * v.x * v.z + v.w * v.y;

		mat(1, 0) = 2 * v.x * v.y + v.w * v.z;
		mat(1, 1) = 1 - 2 * v.x * v.x - 2 * v.z * v.z;
		mat(1, 2) = 2 * v.y * v.z - v.w * v.x;

		mat(2, 0) = 2 * v.x * v.z - v.w * v.y;
		mat(2, 1) = 2 * v.y * v.z + v.w * v.x;
		mat(2, 2) = 1 - 2 * v.x * v.x - 2 * v.y * v.y;

		return (mat);
	}

	void RotateX(float angle)
	{
		float sina( std::sin( angle * 0.5f ));
   		float cosa( std::cos( angle * 0.5f ));

   		const Quaternion q( cosa * v.w - sina*v.x,
                       cosa*v.x + sina*v.w,
                       cosa*v.y - sina*v.z,
                       cosa*v.z + sina*v.y );

   		this->operator=( q );
	}

	void RotateY(float angle)
	{
		float sina( std::sin( angle * 0.5f ));
   		float cosa( std::cos( angle * 0.5f ));

 		  const Quaternion q( cosa * v.w - sina*v.y,
                       cosa*v.x + sina*v.z,
                       cosa*v.y + sina*v.w,
                       cosa*v.z - sina*v.x );

   		this->operator=( q );
	}

	void RotateZ(float angle)
	{
		float sina( std::sin( angle * 0.5f ));
   		float cosa( std::cos( angle * 0.5f ));

  		const Quaternion q( cosa*v.w - sina*v.z,
                       cosa*v.x - sina*v.y,
                       cosa*v.y + sina*v.x,
                       cosa*v.z + sina*v.w );

   		this->operator=( q );
	}

	vec3<float> ToEulerAngle( void )
	{
		double roll;
		double pitch;
		double yaw;

		// roll (x-axis rotation)
		double sinr_cosp = 2.0f * (v.w * v.x + v.y * v.z);
		double cosr_cosp = 1.0f - 2.0f * (v.x * v.x + v.y * v.y);
		roll = atan2(sinr_cosp, cosr_cosp);

		// pitch (y-axis rotation)
		double sinp = 2.0f * (v.w * v.y - v.z * v.x);
		if (fabs(sinp) >= 1)
			pitch = copysign(M_PI / 2, sinp); // use 90 degrees if out of range
		else
		pitch = asin(sinp);

		// yaw (z-axis rotation)
		double siny_cosp = 2.0f * (v.w * v.z + v.x * v.y);
		double cosy_cosp = 1.0f - 2.0f * (v.y * v.y + v.z * v.z);  
		yaw = atan2(siny_cosp, cosy_cosp);

		std::cout << "ToEuler : " << roll * 180/M_PI << " " << pitch * 180/M_PI  << " " << yaw * 180/M_PI  << std::endl;
		return (vec3<float>(roll * 180/M_PI, pitch * 180/M_PI, yaw * 180/M_PI));
	}

	Quaternion(const Quaternion& quat)
	{
		v = quat.v;
	}

	Quaternion operator*(Quaternion const& b)
	{
		Quaternion q(*this);

		return q *= b;
	}

	Quaternion& operator*=(Quaternion const& b)
	{
		this->v.x = v.w * b.v.x	+ this->v.x * b.v.w	+ this->v.y * b.v.z	- this->v.z * b.v.y;
		this->v.y = v.w * b.v.y	- this->v.x * b.v.z	+ this->v.y * b.v.w	+ this->v.z * b.v.x;
		this->v.z = v.w * b.v.z	+ this->v.x * b.v.y	- this->v.y * b.v.x	+ this->v.z * b.v.w;
		this->v.w   = v.w * b.v.w	- this->v.x * b.v.x	- this->v.y * b.v.y	- this->v.z * b.v.z;
		
		return (*this);// = (*this) * b;
	}

	Quaternion& operator=(Quaternion const& b)
	{
		this->v = b.v;

		return *this;
	}

	float Magnitude( void )
	{
		return (sqrt((this->v.x * this->v.x) + (this->v.y * this->v.y) + (this->v.z * this->v.z) + (this->v.w * this->v.w)));
	}

	Quaternion Normalize( void )
	{
		Quaternion Result = *this;
		float mag;

		mag = Magnitude();
		if (mag == 0.0f)
			return Result;
		Result.v.x /= mag;
		Result.v.y /= mag;
		Result.v.z /= mag;
		Result.v.w /= mag;

		return (Result);
	}

	vec4<float> v;
};
