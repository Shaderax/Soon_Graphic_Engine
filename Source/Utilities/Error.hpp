#pragma once

#include <iostream>
#include <sstream>
#include <cstdint>
#include <limits>

template<typename... Args>
inline void Debug(Args... args)
{
	std::ostringstream stream;
	(stream << ... << std::forward<Args>(args));

	std::string path = __FILE__;
	std::cerr << "[" << path.substr(path.find_last_of("/\\") + 1) << "] " << __PRETTY_FUNCTION__ << ":" << __LINE__ << ": " << stream.str() << std::endl;
}

#ifdef NDEBUG 
	#define DEBUG(...) do{}while(0);
#else 
	#define DEBUG(...) Debug(__VA_ARGS__)
#endif

namespace Soon
{
    const std::uint32_t IdError = std::numeric_limits<std::uint32_t>::max();
}