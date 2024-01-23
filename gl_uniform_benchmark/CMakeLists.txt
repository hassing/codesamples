cmake_minimum_required (VERSION 3.16)

project("gl_uniform_benchmark")

SET(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
SET(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
SET(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory("vendor/glfw")

SET(CMAKE_CXX_STANDARD 20)

add_executable(gl_uniform_benchmark WIN32 "vendor/gl_core_4_3.c" "shader_cache.cpp" "gl_uniform_benchmark.cpp")

target_link_libraries(gl_uniform_benchmark glfw)
