cmake_minimum_required(VERSION 3.13.0)
project(vekamp)

#set output dirs
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin/)

if(WIN32)
    set(EXE_PATH ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}debug)
elseif(UNIX)
    set(EXE_PATH ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endif()

# BASS -------------------------------------------------------------------------
set(BASS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vendor/bass24)

message ("== Linking BASS ==")
message ("Looking for LIB at: ${BASS_DIR}/libs/win64/bass.lib")

# Set up BASS Library
add_library(BASS SHARED IMPORTED) 

if (WIN32)
    set_target_properties(BASS PROPERTIES IMPORTED_IMPLIB ${BASS_DIR}/libs/win64/bass.lib)
    set_target_properties(BASS PROPERTIES IMPORTED_LOCATION ${BASS_DIR}/libs/win64/bass.dll)
elseif (UNIX)
    set_target_properties(BASS PROPERTIES IMPORTED_LOCATION ${BASS_DIR}/libs/linux-x86_64/libbass.so)
endif()


# Link library
if (WIN32)
    set(BASS_LIB_PATH ${BASS_DIR}/libs/win64/bass.lib)
    set(BASS_DLL_PATH ${BASS_DIR}/libs/win64/bass.dll)
elseif (UNIX)
    set(BASS_LIB_PATH ${BASS_DIR}/libs/linux-x86_64/libbass.so) 
    set(BASS_DLL_PATH ${BASS_DIR}/libs/linux-x86_64/libbass.so) 
endif()

# GTK4 -------------------------------------------------------------------------
set(GTK_WIN_PATH C:/gtk-build/gtk/x64/release)

if (WIN32)
    set(CMAKE_PREFIX_PATH ${GTK_WIN_PATH})
endif()

# Use the package PkgConfig to detect GTK+ headers/library files
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED gtk4)

# Setup CMake to use GTK+, tell the compiler where to look for headers
# and to the linker where to look for libraries
include_directories(${GTK4_INCLUDE_DIRS})
link_directories(${GTK4_LIBRARY_DIRS})
set(CMAKE_SYSTEM_PREFIX_PATH ${GTK4_LIBRARY_DIRS})

# Add other flags to the compiler
add_definitions(${GTK4_CFLAGS_OTHER})

# MAIN -------------------------------------------------------------------------

add_executable(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} ${GTK4_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${BASS_LIB_PATH}) 
target_include_directories(${PROJECT_NAME} PRIVATE ${BASS_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${GTK4_INCLUDE_DIRS})

target_sources(${PROJECT_NAME} PRIVATE
    src/utils.cpp
    src/basshelpers.cpp
    src/main.cpp
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy ${BASS_DLL_PATH} ${EXE_PATH}
    VERBATIM
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/assets ${EXE_PATH}/assets
    VERBATIM
)