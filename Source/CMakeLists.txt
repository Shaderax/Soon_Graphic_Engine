cmake_minimum_required (VERSION 3.10)

add_compile_options(-Wall -Wextra -g3 -Wshadow -Wnon-virtual-dtor -pedantic)

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
	)

add_library(Source ${SRC})

#target_link_libraries(Core ECS GraphicsGLFW ${VULKAN_LIB} assimp)#Parsers)
target_link_libraries(Source ${Vulkan_LIBRARY} glfw)
