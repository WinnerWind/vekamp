cmake_minimum_required(VERSION 3.13.0)
project(vekamp)


message ("== Linking BASS ==")
message ("Looking for LIB at: ${BASS_DIR}/libs/win64/bass.lib")

# Set up BASS Library
add_library(BASS SHARED IMPORTED) 
set_target_properties(BASS PROPERTIES IMPORTED_IMPLIB ${BASS_DIR}/libs/win64/bass.lib)
set_target_properties(BASS PROPERTIES IMPORTED_LOCATION ${BASS_DIR}/libs/win64/bass.dll)

# Link library
target_link_libraries(${PROJECT_NAME} ${BASS_DIR}/libs/win64/bass.lib) 
target_include_directories(${PROJECT_NAME} PRIVATE ${BASS_DIR})

