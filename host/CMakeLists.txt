add_library(asio INTERFACE)
target_include_directories(asio INTERFACE lib/asio-1.18.1/include)


file(GLOB srcs src/*.cpp)
add_executable(payload ${srcs})
target_include_directories(
    payload
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(payload asio)
