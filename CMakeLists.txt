cmake_minimum_required(VERSION 3.7)
project(clion_modern_opengl)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES src/cpp/App.cpp src/cpp/GLSLProgram.cpp)
include_directories("include")

find_package(OpenGL REQUIRED)
find_package(GLFW3 REQUIRED)
find_package(glm REQUIRED)

add_executable(clion_modern_opengl ${SOURCE_FILES})

target_link_libraries(clion_modern_opengl ${OPENGL_LIBRARIES} glfw)
