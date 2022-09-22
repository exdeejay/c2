cmake_minimum_required(VERSION 3.15)

project(
    "c2payload"
    VERSION 0.1.0
    LANGUAGES CXX
)

if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
  message(FATAL_ERROR "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there.\n")
endif()

set (CMAKE_CXX_STANDARD 17)
add_compile_options(/MT)
add_link_options(/NODEFAULTLIB:MSVCRT)
add_compile_definitions(_WIN32_WINNT=0x0601)

add_subdirectory("third_party")
add_subdirectory("src/host")
# add_subdirectory("test")
