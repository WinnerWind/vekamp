cmake_minimum_required(VERSION 3.13.0)
project(vekamp)

add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME}
PRIVATE
	utils.cpp
	basshelpers.cpp
	main.cpp
	)

if(WIN32)
    add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy ${BASS_DIR}/libs/win64/bass.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug
        VERBATIM
    )
endif()