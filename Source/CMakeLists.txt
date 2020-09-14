cmake_minimum_required (VERSION 3.10)

# Set default compile flags for GCC
if(CMAKE_COMPILER_IS_GNUCXX)
    message(STATUS "GCC detected, adding compile flags")
	 #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++98 -pedantic -Wall -Wextra")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wextra -Wshadow -Wnon-virtual-dtor")
	#add_compile_options(-Wextra -Wshadow -Wnon-virtual-dtor) 
endif(CMAKE_COMPILER_IS_GNUCXX)

if(NOT MSVC)
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -g3 -pedantic")
endif()

set(GRAPHIC_SOURCE_DIR ${CMAKE_SOURCE_DIR})
configure_file(Configure.h.in Configure.h @ONLY)

set(SRC 
	Materials/GraphicMaterial.cpp

	GraphicsInstance.cpp
	GraphicsRenderer.cpp
	Pipelines/PipelineConf.cpp
	Pipelines/GraphicPipelineConf.cpp
	Pipelines/ComputePipelineConf.cpp

	GLFWInit/Init.cpp
	GLFWInit/Hints.cpp
	GLFWInit/Callback.cpp

	Pipelines/BasePipeline.cpp

	Mesh.cpp

	Vertex.cpp
	Utilities/ReadFile.cpp

	../Modules/SPIRV-Reflect/spirv_reflect.c

	UniformsBufferManager.cpp

	Texture.cpp

	Pipelines/GraphicPipeline.cpp
	Pipelines/ComputePipeline.cpp

	Materials/Material.cpp

	Pipelines/PipelineJson.cpp

	MeshVertex.cpp
	RendererRessource.cpp
	BufferRenderer.cpp
	GpuBuffer.cpp
	)

add_library(Source ${SRC})

#target_link_libraries(Core ECS GraphicsGLFW ${VULKAN_LIB} assimp)#Parsers)
target_link_libraries(Source ${Vulkan_LIBRARY} glfw)

if (WIN32)
	add_custom_command(TARGET Source COMMAND START "" ${CMAKE_BINARY_DIR}/Programs/ShaderCompiler/Source/ShaderCompilerSource ${CMAKE_SOURCE_DIR}/Ressources/Shaders/)
elseif (UNIX)
	add_custom_command(TARGET Source COMMAND ${CMAKE_BINARY_DIR}/Programs/ShaderCompiler/Source/ShaderCompilerSource ${CMAKE_SOURCE_DIR}/Ressources/Shaders/ &)
endif()