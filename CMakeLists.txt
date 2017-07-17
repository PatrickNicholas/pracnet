project(network)
cmake_minimum_required(VERSION 2.6)

set(CMAKE_BUILD_TYPE "Debug")

include(cmake/config.cmake)

include_directories(./include)

add_subdirectory(src)
add_subdirectory(test)
