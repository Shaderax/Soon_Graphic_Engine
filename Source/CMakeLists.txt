cmake_minimum_required (VERSION 3.10)

add_compile_options(-Wall -Wextra -g3)

set(SRC 
	Materials/ShaderMaterial.cpp

	Renderer/Vulkan/GraphicsInstance.cpp
	Renderer/Vulkan/GraphicsRenderer.cpp
	Renderer/Vulkan/PipelineConf.cpp

	Renderer/GLFW/Init.cpp
	Renderer/GLFW/Hints.cpp
	Renderer/GLFW/Callback.cpp

	Renderer/Pipelines/BasePipeline.cpp

	Renderer/Mesh.cpp

	Utilities/Vertex.cpp
	Utilities/ReadFile.cpp

	../Modules/SPIRV-Reflect/spirv_reflect.c
	)

add_library(Source ${SRC})

#target_link_libraries(Core ECS GraphicsGLFW ${VULKAN_LIB} assimp)#Parsers)
target_link_libraries(Source ${Vulkan_LIBRARY} glfw)
