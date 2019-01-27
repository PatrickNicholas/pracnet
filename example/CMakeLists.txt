include_directories(../src .)

set(PROXY_SOURCES proxy.cpp)
add_executable(proxy ${PROXY_SOURCES})
target_link_libraries(proxy network)

