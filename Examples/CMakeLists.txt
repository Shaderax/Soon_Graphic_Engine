cmake_minimum_required (VERSION 3.10)

set (CMAKE_CXX_STANDARD 17)

project(ex)

add_compile_options(-Wall -Wextra)

add_executable(GraphicEngineTest main.cpp)

target_link_libraries(GraphicEngineTest Source)
