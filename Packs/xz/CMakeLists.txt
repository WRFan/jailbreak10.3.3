project(xz)

cmake_minimum_required(VERSION 3.14.2)

add_executable(xz
	src/args.c
	src/coder.c
	src/file_io.c
	src/hardware.c
	src/main.c
	src/message.c
	src/mytime.c
	src/options.c
	src/signals.c
	src/suffix.c
	src/util.c
	src/tuklib_open_stdxxx.c
	src/tuklib_progname.c
	src/tuklib_exit.c
	src/tuklib_mbstr_width.c
	src/tuklib_mbstr_fw.c
	src/list.c
)

target_compile_definitions(xz PUBLIC HAVE_ENCODERS=1 HAVE_DECODERS=1 HAVE_STDBOOL_H HAVE_STDINT_H HAVE_INTTYPES_H HAVE_STRINGS_H HAVE_MEMORY_H HAVE_FUTIMES HAVE_ENCODER_LZMA2 HAVE_DECODER_LZMA2 HAVE_ENCODER_DELTA HAVE_DECODER_DELTA HAVE_CLOCK_GETTIME HAVE___BUILTIN_BSWAPXX HAVE_SYS_ENDIAN_H HAVE___BUILTIN_ASSUME_ALIGNED HAVE_MBRTOWC HAVE_WCWIDTH HAVE_OPTRESET MYTHREAD_POSIX NDEBUG SIZEOF_SIZE_T=8 ASSUME_RAM=1024 PACKAGE='xz' PACKAGE_NAME="XZ Utils" PACKAGE_BUGREPORT="lasse.collin@tukaani.org" PACKAGE_URL="https://tukaani.org/xz/") # HAVE_UTIME HAVE_PTHREAD_CONDATTR_SETCLOCK HAVE_DECL_CLOCK_MONOTONIC TUKLIB_FAST_UNALIGNED_ACCESS TUKLIB_USE_UNSAFE_TYPE_PUNNING

target_link_libraries(xz PUBLIC
	#/usr/include/iPhoneOS10.3.sdk/usr/lib/liblzma.5.tbd
	/usr/local/lib/liblzma.5.dylib
)

set_target_properties(xz PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE "/usr/bin")