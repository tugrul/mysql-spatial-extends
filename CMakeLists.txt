cmake_minimum_required(VERSION 3.13)
project(spatial_extends)

# Path for MySQL include directory
include_directories("/usr/include/mysql")

add_definitions("-DHAVE_DLOPEN")
add_library(spatial_extends MODULE spatial_extends.cc spatial_extends.def)
target_link_libraries(spatial_extends)
