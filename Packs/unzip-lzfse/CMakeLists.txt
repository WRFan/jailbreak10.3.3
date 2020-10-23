project(unzip C)

cmake_minimum_required(VERSION 3.14.2)

#cmake_policy (SET CMP0069 NEW)

add_executable(unzip
	crc32.c
	crypt.c
	envargs.c
	explode.c
	extract.c
	fileio.c
	globals.c
	inflate.c
	list.c
	match.c
	process.c
	ttyio.c
	ubz2err.c
	unreduce.c
	unshrink.c
	unzip.c
	zipinfo.c
	unix/unix.c
)

target_include_directories(unzip PUBLIC
	.
	../lzfse/src
)

target_compile_definitions(unzip PUBLIC UNIX USE_LZFSE=1)

set_target_properties(unzip PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE "/usr/bin")

target_link_libraries(unzip PUBLIC
	/usr/lib/liblzfse.dylib
)