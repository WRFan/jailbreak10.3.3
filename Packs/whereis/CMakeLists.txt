project(util-linux)

cmake_minimum_required(VERSION 3.14.2)

#[[
	https://github.com/karelzak/util-linux
]]

add_executable(whereis
	whereis.c
	canonicalize.c
)

target_compile_definitions(whereis PUBLIC HAVE_NANOSLEEP HAVE_FSYNC PACKAGE_STRING="" HAVE_ERR_H HAVE_ERR HAVE_ERRX HAVE_WARN HAVE_WARNX HAVE___PROGNAME HAVE_LOFF_T HAVE_LOCALE_H HAVE_LANGINFO_H HAVE_PATHS_H HAVE_MEMPCPY HAVE_STRNLEN HAVE_STRNDUP HAVE_STRNCHR)

set_target_properties(whereis PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE "/bin")