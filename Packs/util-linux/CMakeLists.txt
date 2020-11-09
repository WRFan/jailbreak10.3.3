project(util-linux)

cmake_minimum_required(VERSION 3.14.2)

#[[
	https://github.com/karelzak/util-linux

	https://manpages.debian.org/unstable/util-linux/index.html

	/usr/include/ncursesw/curses.h
]]

add_executable(whereis
	whereis.c
	canonicalize.c
)

target_compile_definitions(whereis PUBLIC HAVE_NANOSLEEP HAVE_FSYNC PACKAGE_STRING="1.0" HAVE_ERR_H HAVE_ERR HAVE_ERRX HAVE_WARN HAVE_WARNX HAVE___PROGNAME HAVE_LOFF_T HAVE_LOCALE_H HAVE_LANGINFO_H HAVE_PATHS_H HAVE_MEMPCPY HAVE_STRNLEN HAVE_STRNDUP HAVE_STRNCHR)

set_target_properties(whereis PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE "/bin")

###############################################################################
add_executable(look
	look.c
)

target_compile_definitions(look PUBLIC HAVE_NANOSLEEP HAVE_FSYNC PACKAGE_STRING="1.0" HAVE_ERR_H HAVE_ERR HAVE_ERRX HAVE_WARN HAVE_WARNX HAVE___PROGNAME HAVE_LOFF_T HAVE_LOCALE_H HAVE_LANGINFO_H HAVE_PATHS_H HAVE_MEMPCPY HAVE_STRNLEN HAVE_STRNDUP HAVE_STRNCHR)

set_target_properties(look PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE "/bin")

###############################################################################
add_executable(pg
	pg.c
	strutils.c
)

# target_include_directories(pg PUBLIC
	# /usr/include/
# )

target_compile_definitions(pg PUBLIC HAVE_NANOSLEEP HAVE_FSYNC PACKAGE_STRING="1.0" PACKAGE_VERSION=PACKAGE_STRING HAVE_ERR_H HAVE_ERR HAVE_ERRX HAVE_WARN HAVE_WARNX HAVE___PROGNAME HAVE_LOFF_T HAVE_LOCALE_H HAVE_LANGINFO_H HAVE_PATHS_H HAVE_MEMPCPY HAVE_STRNLEN HAVE_STRNDUP HAVE_STRNCHR HAVE_NCURSESW_NCURSES_H HAVE_NCURSESW_TERM_H HAVE_FSEEKO HAVE_WIDECHAR)

set_target_properties(pg PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE "/bin")

target_link_libraries(pg PUBLIC
	/usr/include/iPhoneOS10.3.sdk/usr/lib/libncurses.5.4.tbd
)