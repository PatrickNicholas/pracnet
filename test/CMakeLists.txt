include_directories(../src .)

set(IP_SOURCES main.cpp 
    ip.cpp)
add_executable(testip ${IP_SOURCES})
target_link_libraries(testip ip)

add_custom_target(testpath_ip)
add_custom_command(TARGET testpath_ip
    COMMAND testip )