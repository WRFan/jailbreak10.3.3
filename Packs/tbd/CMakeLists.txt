project(tbd C)

cmake_minimum_required(VERSION 3.14.2)

#SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -funroll-loops")

file(GLOB SRC src/*.c)

add_executable(tbd ${SRC})

target_include_directories(tbd PUBLIC
	include
)

set_target_properties(tbd PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE "/bin")