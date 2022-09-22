file(GLOB_RECURSE srcs "*.cpp")
add_library(payload ${srcs})
target_include_directories(payload PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(payload asio lodepng PortAudio snappy zlib)

set_property(TARGET payload PROPERTY WIN32_EXECUTABLE $<IF:$<CONFIG:Debug>,OFF,ON>)
target_compile_definitions(payload PUBLIC WIN32_LEAN_AND_MEAN)
target_compile_definitions(payload PUBLIC $<$<NOT:$<CONFIG:Debug>>:WINGUI>)
