#include "Callback.hpp"
#include <iostream>

namespace Soon
{
	void error_callback(int error, const char* description)
	{
		std::cerr << "Error: " << description << std::endl;
	}
}
