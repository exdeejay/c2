add_executable(c2stager "stager.c")
target_link_libraries(c2stager PRIVATE MemoryModule)
target_compile_definitions(c2stager PUBLIC $<$<CONFIG:Debug>:DEBUG>)

if(WIN32)
    set_property(TARGET c2 PROPERTY WIN32_EXECUTABLE $<IF:$<CONFIG:Debug>,OFF,ON>)
    target_compile_definitions(c2 PUBLIC WIN32_LEAN_AND_MEAN)
    target_compile_definitions(c2 PUBLIC $<$<NOT:$<CONFIG:Debug>>:WINGUI>)
endif()
