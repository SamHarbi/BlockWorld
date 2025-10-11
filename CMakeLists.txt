cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build)

project(BlockWorld VERSION 1.0)
add_executable(BlockWorld src/BlockWorld.cpp src/ChunkBlock.cpp src/cube_tex.cpp src/glad.c src/ModelLoader/tiny_loader_texture.cpp src/wrapper_glfw.cpp)
target_include_directories(BlockWorld PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/)
target_link_libraries( BlockWorld )

# Emscripten-specific configurations
set(USE_GLFW_PORT_FLAGS "-sUSE_GLFW=3")
set(PACK_FILES "--embed-file")
set(FILES_TO_PACK "src/Assets@/")
set(EMC_FLAGS "-sFULL_ES3=1 -sWASM=3 -sUSE_WEBGL2=1 -sWASM_BIGINT -g")

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${USE_GLFW_PORT_FLAGS} ${PACK_FILES} ${FILES_TO_PACK}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${USE_GLFW_PORT_FLAGS} ${PACK_FILES} ${FILES_TO_PACK} ${EMC_FLAGS}")

# Set executable suffix for web targets
set(CMAKE_EXECUTABLE_SUFFIX .html)