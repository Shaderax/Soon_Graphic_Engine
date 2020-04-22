cmake_minimum_required (VERSION 3.10)

set(SRC 
	ShaderMaterial.cpp
	)

add_library(Materials ${SRC})

#target_link_libraries(Materials Scene)
