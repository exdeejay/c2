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

add_library(asio INTERFACE)
target_include_directories(asio INTERFACE "${CMAKE_SOURCE_DIR}/lib/asio")

add_library(lodepng "${CMAKE_SOURCE_DIR}/lib/lodepng/lodepng.cpp")
target_include_directories(lodepng PUBLIC "${CMAKE_SOURCE_DIR}/lib/lodepng")

add_library(portaudio STATIC IMPORTED)
set_property(TARGET portaudio PROPERTY IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/lib/portaudio/portaudio_static_x64.lib")
target_include_directories(portaudio INTERFACE "${CMAKE_SOURCE_DIR}/lib/portaudio/include")


file(GLOB_RECURSE srcs "src/*.cpp")
add_executable(payload ${srcs}) # WIN32
target_include_directories(
    payload
    PUBLIC
        "$<INSTALL_INTERFACE:include>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
target_link_libraries(payload asio lodepng portaudio)
#set(CMAKE_FIND_LIBRARY_SUFFIXES ".lib")
set_property(TARGET payload PROPERTY WIN32_EXECUTABLE $<IF:$<CONFIG:Debug>,OFF,ON>)
target_compile_definitions(payload PUBLIC ASIO_SEPARATE_COMPILATION _WIN32_WINNT=0x0601 WIN32_LEAN_AND_MEAN $<$<NOT:$<CONFIG:Debug>>:WINGUI>)
