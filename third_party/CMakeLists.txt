add_library(asio INTERFACE)
target_include_directories(asio INTERFACE "asio/asio/include")

add_library(fpng "fpng/src/fpng.cpp")
target_include_directories(fpng PUBLIC "fpng/src")

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
#TODO: disable install
add_subdirectory("portaudio")

add_subdirectory("SPSCQueue")

set(SNAPPY_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(SNAPPY_BUILD_BENCHMARKS OFF CACHE BOOL "" FORCE)
set(SNAPPY_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory("snappy")

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(SKIP_INSTALL_ALL ON CACHE BOOL "" FORCE)
add_subdirectory("zlib")
target_include_directories(zlib INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/zlib" "${PROJECT_BINARY_DIR}/third_party/zlib")
target_include_directories(zlibstatic INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/zlib" "${PROJECT_BINARY_DIR}/third_party/zlib")

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
add_subdirectory("googletest")
