#pragma once

#include "Modules/Math/mat4.hpp"

struct UniformCamera
{
	alignas(16) mat4<float> view;
	alignas(16) mat4<float> proj;
};

struct UniformModel
{
	alignas(16) mat4<float> model;
};