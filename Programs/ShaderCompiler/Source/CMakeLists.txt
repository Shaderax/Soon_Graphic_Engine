cmake_minimum_required(VERSION 3.0)

project(ShaderCompilerSource)

include_directories(.)

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	add_executable(ShaderCompilerSource main.cpp Windows/ShaderCompilerWin.cpp)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	add_executable(ShaderCompilerSource main.cpp Linux/ShaderCompilerLinux.cpp)
endif()

target_link_libraries(ShaderCompilerSource efsw)