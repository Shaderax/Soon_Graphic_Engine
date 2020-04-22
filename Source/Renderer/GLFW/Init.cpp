#include "Init.hpp"
#include "Callback.hpp"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <iostream>

namespace Soon
{
	bool InitGLFW( void )
	{
		glfwSetErrorCallback(error_callback);
		if (!glfwInit())
		{
		//	SOON_ERR_THROW(0, "Failed to initialize GLFW");
			return (false);
		}
		return (true);
	}
}
