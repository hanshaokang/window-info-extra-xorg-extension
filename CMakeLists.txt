cmake_minimum_required(VERSION 3.14)

project(window_info_extra_xorg_extension C CXX)

set(CMAKE_C_STANDARD 11)

set(CMAKE_VERBOSE_MAKEFILE ON)

add_subdirectory(common)
add_subdirectory(extension)
add_subdirectory(xlib)
add_subdirectory(test)