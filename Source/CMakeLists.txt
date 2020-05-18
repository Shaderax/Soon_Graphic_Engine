cmake_minimum_required (VERSION 3.10)

# Set default compile flags for GCC
if(CMAKE_COMPILER_IS_GNUCXX)
    message(STATUS "GCC detected, adding compile flags")
     #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++98 -pedantic -Wall -Wextra")
	add_compile_options(-Wextra -Wshadow -Wnon-virtual-dtor) 
endif(CMAKE_COMPILER_IS_GNUCXX)
add_compile_options(-Wall -g3 -pedantic)

set(SRC 
	Materials/ShaderMaterial.cpp

	GraphicsInstance.cpp
	GraphicsRenderer.cpp
	PipelineConf.cpp

	GLFWInit/Init.cpp
	GLFWInit/Hints.cpp
	GLFWInit/Callback.cpp

	Pipelines/BasePipeline.cpp

	Mesh.cpp

	Vertex.cpp
	Utilities/ReadFile.cpp

	../Modules/SPIRV-Reflect/spirv_reflect.c

	UniformsBufferManager.cpp
	)

add_library(Source ${SRC})

#target_link_libraries(Core ECS GraphicsGLFW ${VULKAN_LIB} assimp)#Parsers)
target_link_libraries(Source ${Vulkan_LIBRARY} glfw)
