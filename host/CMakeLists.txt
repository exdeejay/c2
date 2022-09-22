cmake_minimum_required(VERSION 3.15)

project(
    "c2payload"
    VERSION 0.1.0
    LANGUAGES CXX
) 

if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
  message(FATAL_ERROR "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there.\n")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(/MD)
else()
    add_compile_options(/MT)
endif()

add_library(asio INTERFACE)
target_include_directories(asio INTERFACE lib/asio-1.18.1/include)

add_library(lodepng lib/lodepng/lodepng.cpp)
target_include_directories(lodepng PUBLIC lib/lodepng)

file(GLOB_RECURSE srcs src/*.cpp)
add_executable(payload WIN32 ${srcs}) # WIN32
target_include_directories(
    payload
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(payload asio lodepng)
set(CMAKE_FIND_LIBRARY_SUFFIXES ".lib")
target_compile_definitions(payload PUBLIC _WIN32_WINNT=0x0601 WIN32_LEAN_AND_MEAN)
