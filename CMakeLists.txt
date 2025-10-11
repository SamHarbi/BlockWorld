cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build)

project(BlockWorld VERSION 1.0)
add_executable(BlockWorld src/BlockWorld.cpp src/ChunkBlock.cpp src/cube_tex.cpp src/glad.c src/ModelLoader/tiny_loader_texture.cpp src/wrapper_glfw.cpp)
target_include_directories(BlockWorld PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/)
target_link_libraries( BlockWorld )

find_package(glfw3 3.3 REQUIRED)
target_link_libraries(BlockWorld glfw)