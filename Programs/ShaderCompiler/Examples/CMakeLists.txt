cmake_minimum_required(VERSION 3.0)

project(Examples)

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	add_executable(FakeGame FakeGameWin.cpp)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	add_executable(FakeGame FakeGameLinux.cpp)
endif()