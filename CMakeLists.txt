project(network)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(VERSION_MAJOR 1)
set(VERSION_MINOR 0)

include(cmake/config.cmake)
include(cmake/options.cmake)
include(cmake/depend.cmake)

include_directories(./include)

add_subdirectory(src)
add_subdirectory(test)
