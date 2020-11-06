project(free)

cmake_minimum_required(VERSION 3.14.2)

#[[
	https://github.com/dcantrell/darwin-free

	https://github.com/nickolasburr/homebrew-pfa
]]

add_executable(free free.c)

target_compile_definitions(free PUBLIC _FREE_VERSION="1.0")

set_target_properties(free PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE "/bin")