cmake_minimum_required(VERSION 3.15)

project(
    "c2payload"
    VERSION 0.1.0
    LANGUAGES CXX
) 

if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
  message(FATAL_ERROR "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there.\n")
endif()


add_library(asio INTERFACE)
target_include_directories(asio INTERFACE host/lib/asio-1.18.1/include)


file(GLOB srcs host/src/*.cpp)
add_executable(payload ${srcs})
target_include_directories(
    payload
    PUBLIC
        $<INSTALL_INTERFACE:host/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/host/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/host/src
)
target_link_libraries(payload asio)
set(CMAKE_FIND_LIBRARY_SUFFIXES ".lib")
target_compile_options(payload PUBLIC /MT)
target_link_options(payload PUBLIC /INCREMENTAL:NO /NODEFAULTLIB:MSVCRT)
