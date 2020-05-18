cmake_minimum_required (VERSION 3.10)

#project(ex)

# Set default compile flags for GCC
if(CMAKE_COMPILER_IS_GNUCXX)
	message(STATUS "GCC detected, adding compile flags")
	add_compile_options(-Wextra) 
endif(CMAKE_COMPILER_IS_GNUCXX)
add_compile_options(-Wall)

add_executable(GraphicEngineTest main.cpp)

target_link_libraries(GraphicEngineTest Source)
