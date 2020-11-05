project(pidof)

cmake_minimum_required(VERSION 3.14.2)

#[[
	http://www.nightproductions.net/cli.htm

	https://opensource.apple.com/source/Libc/Libc-1353.41.1/include/signal.h.auto.html

	!!!
	https://askubuntu.com/questions/361104/pgrep-pattern-length-limit
]]

add_executable(pidof pidof.c)

set_target_properties(pidof PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE "/bin")