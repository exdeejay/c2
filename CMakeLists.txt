cmake_minimum_required(VERSION 3.15)

project(
    "c2payload"
    VERSION 0.1.0
    LANGUAGES CXX
)

if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
  message(FATAL_ERROR "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there.\n")
endif()

set(CMAKE_SYSTEM_NAME "Windows")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_COMPILER "/usr/bin/x86_64-w64-mingw32-gcc" CACHE STRING "" FORCE)
set(CMAKE_C_COMPILER_WORKS 1)
set(CMAKE_CXX_COMPILER "/usr/bin/x86_64-w64-mingw32-g++" CACHE STRING "" FORCE)
set(CMAKE_CXX_COMPILER_WORKS 1)
set(CMAKE_FIND_ROOT_PATH "/usr/x86_64-w64-mingw32")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

add_subdirectory("third_party")

if(WIN32 AND MSVC)
  add_compile_options(/MT)
  add_compile_definitions(_WIN32_WINNT=0x0601)
endif()

add_subdirectory("host")
