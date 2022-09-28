file(GLOB_RECURSE srcs "src/*.cpp")
add_library(payload STATIC ${srcs})
target_include_directories(payload PUBLIC "include")
set(include_dirs "src" "src/commands" "src/connection" "src/controller")
target_include_directories(payload PRIVATE ${include_dirs})
target_link_libraries(payload asio lodepng PortAudio snappy zlibstatic)

set_property(TARGET payload PROPERTY WIN32_EXECUTABLE $<IF:$<CONFIG:Debug>,OFF,ON>)
target_compile_definitions(payload PUBLIC WIN32_LEAN_AND_MEAN)
target_compile_definitions(payload PUBLIC $<$<NOT:$<CONFIG:Debug>>:WINGUI>)
target_compile_definitions(payload PUBLIC $<$<CONFIG:Debug>:DEBUG>)

add_executable(payload_main "src/main.cpp")
target_link_libraries(payload_main payload)

add_executable(util_test "test/util.cpp")
target_link_libraries(util_test gtest_main payload)
