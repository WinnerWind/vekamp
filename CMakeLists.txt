cmake_minimum_required(VERSION 3.13.0)
project(vekamp)


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin/)
set(BASS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vendor/bass24)

add_subdirectory(src)
add_subdirectory(vendor)