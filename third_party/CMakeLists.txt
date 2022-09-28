add_library(asio INTERFACE)
target_include_directories(asio INTERFACE "asio/asio/include")

add_library(lodepng "lodepng/lodepng.cpp")
target_include_directories(lodepng INTERFACE "lodepng")

cmake_policy(SET CMP0077 NEW)

set(BUILD_SHARED_LIBS OFF)
#TODO: disable install
add_subdirectory("portaudio")

set(SNAPPY_BUILD_TESTS OFF)
set(SNAPPY_BUILD_BENCHMARKS OFF)
set(SNAPPY_INSTALL OFF)
add_subdirectory("snappy")

set(BUILD_SHARED_LIBS OFF)
set(SKIP_INSTALL_ALL ON)
add_subdirectory("zlib")
target_include_directories(zlibstatic INTERFACE "zlib" "${PROJECT_BINARY_DIR}/third_party/zlib")

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
add_subdirectory("googletest")
