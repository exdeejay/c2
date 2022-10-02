set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)

### Premade CMakeLists.txt ###
# MemoryModule
add_subdirectory("MemoryModule")
# portaudio
add_subdirectory("portaudio")
# opus
add_subdirectory("opus")
# SPSCQueue
add_subdirectory("SPSCQueue")
# snappy
set(SNAPPY_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(SNAPPY_BUILD_BENCHMARKS OFF CACHE BOOL "" FORCE)
set(SNAPPY_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory("snappy")
# zlib
set(SKIP_INSTALL_ALL ON CACHE BOOL "" FORCE)
add_subdirectory("zlib")
target_include_directories(zlib INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/zlib" "${PROJECT_BINARY_DIR}/third_party/zlib")
target_include_directories(zlibstatic INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/zlib" "${PROJECT_BINARY_DIR}/third_party/zlib")
# googletest
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
add_subdirectory("googletest")


### Manually added libraries ###

# asio
add_library(asio INTERFACE)
target_include_directories(asio INTERFACE "asio/asio/include")

# fpng
add_library(fpng "fpng/src/fpng.cpp")
target_include_directories(fpng PUBLIC "fpng/src")

# libopusenc
file(GLOB_RECURSE libopusenc_srcs "libopusenc/src/*.c")
add_library(libopusenc ${libopusenc_srcs})
target_include_directories(libopusenc
    PUBLIC "libopusenc/include"
    PRIVATE "libopusenc/src"
)
target_compile_definitions(libopusenc PRIVATE
    PACKAGE_NAME="libopusenc"
    PACKAGE_VERSION="v0.2.1-14-gf51c3aa"
    RANDOM_PREFIX=libopusenc 
    OUTSIDE_SPEEX
)
target_link_libraries(libopusenc PUBLIC opus)
