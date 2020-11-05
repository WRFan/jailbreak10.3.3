project(proctools)

cmake_minimum_required(VERSION 3.14.2)

#[[
	proctools-0.4pre1:
	http://proctools.sourceforge.net

	http://proctools.sourceforge.net/pgrep.html
	http://proctools.sourceforge.net/pfind.html
	http://proctools.sourceforge.net/pkill.html

	procps-ng:
	https://gitlab.com/procps-ng/procps

	https://opensource.apple.com/source/apache/apache-786.1/httpd/srclib/pcre/pgrep.c.auto.html

	https://opensource.apple.com/source/Libc/Libc-1353.41.1/include/kvm.h.auto.html
	https://opensource.apple.com/source/Libc/Libc-1353.41.1/include/nlist.h.auto.html

	https://opensource.apple.com/source/xnu/xnu-3789.70.16/bsd/sys/user.h.auto.html
	https://opensource.apple.com/source/xnu/xnu-3789.70.16/bsd/sys/ptrace.h.auto.html
]]

add_executable(pgrep
	pgrep.c
	proctools.c
	fmt.c
)

target_include_directories(pgrep PUBLIC
	/usr/include/iPhoneOS10.3.sdk/usr/local/include/Libc

	/usr/include/iPhoneOS10.3.sdk/usr/local/include/bsd
)

target_compile_definitions(pgrep PUBLIC KERN_PROCARGS2 _AOUT_INCLUDE_) # USE_SYSCTL

set_target_properties(pgrep PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE "/bin")

###############################################################################
add_executable(pfind
	pfind.c
	proctools.c
	fmt.c
)

target_include_directories(pfind PUBLIC
	/usr/include/iPhoneOS10.3.sdk/usr/local/include/Libc

	/usr/include/iPhoneOS10.3.sdk/usr/local/include/bsd
)

target_compile_definitions(pfind PUBLIC KERN_PROCARGS2 _AOUT_INCLUDE_)

set_target_properties(pfind PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE "/bin")

###############################################################################
add_executable(pkill
	pkill.c
	proctools.c
)

target_include_directories(pkill PUBLIC
	/usr/include/iPhoneOS10.3.sdk/usr/local/include/Libc

	/usr/include/iPhoneOS10.3.sdk/usr/local/include/bsd
)

target_compile_definitions(pkill PUBLIC KERN_PROCARGS2 _AOUT_INCLUDE_)

set_target_properties(pkill PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE "/bin")