#include <iostream>

namespace Soon
{
	template <typename ...Args>
	void Log(Args&& ...args)
	{
	    std::ostringstream stream;
		(stream << ... << std::forward<Args>(args));
	    std::cout << stream.str() << std::endl;;
	}
};