cmake_minimum_required(VERSION 3.13.0)
project(vekamp)

add_subdirectory(bass24)

SET(CMAKE_PREFIX_PATH C:/gtk-build/gtk/x64/release/)
# Use the package PkgConfig to detect GTK+ headers/library files
FIND_PACKAGE(PkgConfig REQUIRED)
pkg_check_modules (GTK4 REQUIRED gtk4)

# Setup CMake to use GTK+, tell the compiler where to look for headers
# and to the linker where to look for libraries
INCLUDE_DIRECTORIES(${GTK4_INCLUDE_DIRS})
LINK_DIRECTORIES(${GTK4_LIBRARY_DIRS})

MESSAGE("GTK4 Include Dirs: ${GTK4_INCLUDE_DIRS}")
MESSAGE("GTK4 Library Dirs: ${GTK4_LIBRARY_DIRS}")
MESSAGE("GTK4 Libraries: ${GTK4_LIBRARIES}")

# Add other flags to the compiler
ADD_DEFINITIONS(${GTK4_CFLAGS_OTHER})

#target_include_directories(${PROJECT_NAME} PRIVATE ${GTK4_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${GTK4_LIBRARIES})