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

add_library(asio "${CMAKE_SOURCE_DIR}/lib/asio/asio.cpp")
target_include_directories(asio PUBLIC "${CMAKE_SOURCE_DIR}/lib/asio")
target_compile_definitions(asio PRIVATE ASIO_SEPARATE_COMPILATION)

add_library(lodepng "${CMAKE_SOURCE_DIR}/lib/lodepng/lodepng.cpp")
target_include_directories(lodepng PUBLIC "${CMAKE_SOURCE_DIR}/lib/lodepng")

add_library(portaudio STATIC IMPORTED)
set_property(TARGET portaudio PROPERTY IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/lib/portaudio/portaudio_static_x64.lib")
target_include_directories(portaudio INTERFACE "${CMAKE_SOURCE_DIR}/lib/portaudio/include")

add_library(snappy STATIC IMPORTED)
set_property(TARGET snappy PROPERTY IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/lib/snappy/snappy.lib")
target_include_directories(snappy INTERFACE "${CMAKE_SOURCE_DIR}/lib/snappy")

add_library(zlib STATIC IMPORTED)
set_property(TARGET zlib PROPERTY IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/lib/zlib/zlibstatic.lib")
target_include_directories(zlib INTERFACE "${CMAKE_SOURCE_DIR}/lib/zlib")


file(GLOB_RECURSE srcs "src/*.cpp")
add_library(payloadlib ${srcs})
target_include_directories(payloadlib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_link_libraries(payloadlib asio lodepng portaudio snappy zlib)
target_compile_definitions(payloadlib PUBLIC WIN32_LEAN_AND_MEAN)

add_executable(payload "main.cpp")
target_link_libraries(payload payloadlib)
set_property(TARGET payload PROPERTY WIN32_EXECUTABLE $<IF:$<CONFIG:Debug>,OFF,ON>)
target_compile_definitions(payload PUBLIC $<$<NOT:$<CONFIG:Debug>>:WINGUI>)

add_executable(test "test/test.cpp")
target_link_libraries(test payloadlib)
