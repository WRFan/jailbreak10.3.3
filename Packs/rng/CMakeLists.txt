project(rng)

cmake_minimum_required(VERSION 3.14.2)

#[[
	https://github.com/nickolasburr/rng
]]

add_executable(rng
	argv.c
	assert.c
	error.c
	except.c
	main.c
	mem.c
	utils.c
)

target_compile_definitions(rng PUBLIC __APPLE__)

set_target_properties(rng PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE "/bin")