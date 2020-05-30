cmake_minimum_required (VERSION 3.10)

set(SOURCE
	Init.cpp
	Hints.cpp
	Callback.cpp
	)

add_library(GraphicsGLFW ${SOURCE})
target_link_libraries(GraphicsGLFW glfw)
