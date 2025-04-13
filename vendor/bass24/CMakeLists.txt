cmake_minimum_required(VERSION 3.13.0)
project(vekamp)


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
    target_link_libraries(${PROJECT_NAME} ${BASS_DIR}/libs/win64/bass.lib) 
elseif (UNIX)
    target_link_libraries(${PROJECT_NAME} ${BASS_DIR}/libs/linux-x86_64/libbass.so) 
endif()

target_include_directories(${PROJECT_NAME} PRIVATE ${BASS_DIR})

