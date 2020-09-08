cmake_minimum_required (VERSION 3.10)

set(CMAKE_CXX_COMPILER g++)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CC_COMPILER gcc)
set(CMAKE_C_STANDARD 11)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/CMake")

project(Soon_Graphic_Engine)

if (NOT MSVC)
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address -g3")
endif()

find_package(Vulkan REQUIRED)

if (Vulkan_FOUND)
	message(${Vulkan_INCLUDE_DIR})
	message(${Vulkan_LIBRARY})
endif()

message(${CMAKE_CXX_FLAGS_DEBUG})
message(${CMAKE_CXX_FLAGS_RELEASE})

#set(ENV{VK_ICD_FILENAMES} "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/MoltenVK/macOS/MoltenVK_icd.json")
#set(ENV{VK_LAYER_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/vulkan/Validation_Layer/explicit_layer.d/macos/")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}) 
include_directories(Source)
include_directories(SYSTEM ThirdParty)
include_directories(SYSTEM ThirdParty/VkMemoryAllocator)
include_directories(SYSTEM ThirdParty/glfw/include)
include_directories(SYSTEM ThirdParty/glm)
include_directories(SYSTEM Modules)
include_directories(${PROJECT_BINARY_DIR}/Source)

#include_directories(/usr/include/)
#include_directories(ThirdParty/)

add_subdirectory(ThirdParty)
add_subdirectory(Modules)
add_subdirectory(Source)
add_subdirectory(Examples)

message("CMAKE_CURRENT_SOURCE_DIR : ${CMAKE_CURRENT_SOURCE_DIR}")
message("CMAKE_BINARY_DIR : ${CMAKE_BINARY_DIR}")
message("CMAKE_SOURCE_DIR : " ${CMAKE_SOURCE_DIR})
message("CMAKE_CURRENT_BINARY_DIR : ${CMAKE_CURRENT_BINARY_DIR}")

#42 Mac :
#VK_ICD_FILENAMES=/Users/nrouzeva/Documents/42_PJ/Soon_engine/ThirdParty/MoltenVK/macOS/MoltenVK_icd.json
#LD_LIBRARY_PATH=:/Users/nrouzeva/Documents/42_PJ/Soon_engine/ThirdParty/vulkan/libs/macos
#VK_LAYER_PATH=/Users/nrouzeva/Documents/42_PJ/Soon_engine/ThirdParty/vulkan/Validation_Layer/explicit_layer.d/macos/

#linux :
#VK_LAYER_PATH=/home/shaderax/Documents/Project/Soon_engine/ThirdParty/vulkan/Validation_Layer/explicit_layer.d/linux/
