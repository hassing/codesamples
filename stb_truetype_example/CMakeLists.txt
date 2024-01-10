cmake_minimum_required (VERSION 3.16)

project("stb_truetype_example")

SET(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
SET(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
SET(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory("vendor/glfw")

add_executable(stb_truetype_example WIN32 "vendor/gl_core_4_3.c" "stb_truetype_example.cpp")

target_link_libraries(stb_truetype_example glfw)
