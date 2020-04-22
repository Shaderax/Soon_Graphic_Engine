cmake_minimum_required (VERSION 3.10)

set (SRC
	GraphicsInstance.cpp
	GraphicsRenderer.cpp
	PipelineConf.cpp
	)

add_library(GraphicsVulkan
	${SRC}
	)

if(APPLE)
	find_library(
		VULKAN_LIB
		NAMES libvulkan.1.1.101.dylib libvulkan.dylib
		HINTS "../ThirdParty/vulkan/libs/macos/"
		NO_DEFAULT_PATH
		)

	find_library(
		MOLTENVK_LIB
		NAMES libMoltenVK.dylib
		HINTS "../ThirdParty/MoltenVK/macOS/"
		NO_DEFAULT_PATH
		)

	target_link_libraries(GraphicsVulkan glfw ${VULKAN_LIB})

elseif(UNIX)
	find_library(
		VULKAN_LIB
		NAMES libvulkan.so
		HINTS "../ThirdParty/vulkan/libs/linux/"
		NO_DEFAULT_PATH
		)
	target_link_libraries(GraphicsVulkan glfw ${VULKAN_LIB})

endif()

#42 Mac :
#VK_ICD_FILENAMES=/Users/nrouzeva/Documents/42_PJ/Soon_engine/ThirdParty/MoltenVK/macOS/MoltenVK_icd.json
#LD_LIBRARY_PATH=:/Users/nrouzeva/Documents/42_PJ/Soon_engine/ThirdParty/vulkan/libs/macos
#VK_LAYER_PATH=/Users/nrouzeva/Documents/42_PJ/Soon_engine/ThirdParty/vulkan/Validation_Layer/explicit_layer.d/macos/

#linux :
#VK_LAYER_PATH=/home/shaderax/Documents/Project/Soon_engine/ThirdParty/vulkan/Validation_Layer/explicit_layer.d/linux/
