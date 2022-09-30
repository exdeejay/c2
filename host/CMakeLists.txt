file(GLOB_RECURSE srcs "src/*.cpp")
add_executable(payload ${srcs})
target_include_directories(payload PUBLIC "include")
set(include_dirs "src" "src/commands" "src/connection" "src/controller")
target_include_directories(payload PRIVATE ${include_dirs})
target_link_libraries(payload asio fpng PortAudio SPSCQueue snappy zlibstatic)

target_compile_definitions(payload PUBLIC $<$<CONFIG:Debug>:DEBUG>)
if(WIN32)
    set_property(TARGET payload PROPERTY WIN32_EXECUTABLE $<IF:$<CONFIG:Debug>,OFF,ON>)
    target_compile_definitions(payload PUBLIC WIN32_LEAN_AND_MEAN)
    target_compile_definitions(payload PUBLIC $<$<NOT:$<CONFIG:Debug>>:WINGUI>)
endif()

add_library(payload_util INTERFACE)
target_include_directories(payload_util INTERFACE "include")

add_executable(util_test "test/util.cpp")
target_link_libraries(util_test payload_util gtest_main)
