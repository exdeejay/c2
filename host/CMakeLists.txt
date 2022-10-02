file(GLOB_RECURSE srcs "core/src/*.cpp")
add_library(c2core ${srcs})
target_include_directories(c2core
    PUBLIC "core/include"
    PRIVATE "core/src"
)
target_link_libraries(c2core asio zlibstatic)
target_compile_definitions(c2core PUBLIC $<$<CONFIG:Debug>:DEBUG>)

file(GLOB_RECURSE plugin_srcs "stdplugin/*.cpp")
add_library(stdplugin ${plugin_srcs})
target_include_directories(stdplugin PUBLIC "stdplugin")
target_link_libraries(stdplugin c2core fpng opus PortAudio SPSCQueue snappy)

add_executable(c2 "main.cpp")
target_link_libraries(c2 c2core stdplugin)
if(WIN32)
    set_property(TARGET c2 PROPERTY WIN32_EXECUTABLE $<IF:$<CONFIG:Debug>,OFF,ON>)
    target_compile_definitions(c2 PUBLIC WIN32_LEAN_AND_MEAN)
    target_compile_definitions(c2 PUBLIC $<$<NOT:$<CONFIG:Debug>>:WINGUI>)
endif()

# add_library(payload_util INTERFACE)
# target_include_directories(payload_util INTERFACE "include")

# add_executable(util_test "test/util.cpp")
# target_link_libraries(util_test payload_util gtest_main)
