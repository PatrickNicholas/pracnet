include_directories(../src .)

set(IP_SOURCES main.cpp
    ip.cpp)
add_executable(testip ${IP_SOURCES})
target_link_libraries(testip ip)

set(EXAMPLE_SOURCES example.cpp)
add_executable(example ${EXAMPLE_SOURCES})
target_link_libraries(example ip)

set(EXAMPLE_SOURCES echo.cpp)
add_executable(echo ${EXAMPLE_SOURCES})
target_link_libraries(echo network)

set(EXAMPLE_SOURCES close.cpp)
add_executable(close ${EXAMPLE_SOURCES})
target_link_libraries(close network)

add_custom_target(testpath_ip)
add_custom_command(TARGET testpath_ip
    COMMAND testip )
