#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "Hints.hpp"

namespace Soon
{
	void InitGLFWHints( void )
	{
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	}
}
