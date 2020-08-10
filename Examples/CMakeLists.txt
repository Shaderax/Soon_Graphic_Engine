cmake_minimum_required (VERSION 3.10)

#project(ex)

# Set default compile flags for GCC
if(CMAKE_COMPILER_IS_GNUCXX)
	message(STATUS "GCC detected, adding compile flags")
	#add_compile_options(-Wextra) 
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wextra")
endif(CMAKE_COMPILER_IS_GNUCXX)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
#add_compile_options(-Wall)

add_subdirectory(ThirdParty/tinyobjloader)

#add_executable(skybox skybox.cpp)
#add_executable(particles particles.cpp)
add_executable(postProcessing postProcessing.cpp)

#target_link_libraries(skybox Source tinyobjloader)
#target_link_libraries(particles Source tinyobjloader)
target_link_libraries(postProcessing Source tinyobjloader)
