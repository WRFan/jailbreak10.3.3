project(lzfse C)

cmake_minimum_required(VERSION 3.14.2)

#cmake_policy (SET CMP0063 NEW)
#cmake_policy (SET CMP0042 NEW)
#cmake_policy (SET CMP0069 NEW)

add_library(lzfse
	src/lzfse_decode.c
	src/lzfse_decode_base.c
	src/lzfse_encode.c
	src/lzfse_encode_base.c
	src/lzfse_fse.c
	src/lzvn_decode_base.c
	src/lzvn_encode_base.c
)

set_target_properties(lzfse PROPERTIES C_STANDARD 99 POSITION_INDEPENDENT_CODE TRUE LIBRARY_OUTPUT_DIRECTORY_RELEASE "/usr/lib")

###############################################################################
add_executable(lzfse_cli src/lzfse_main.c)

target_link_libraries(lzfse_cli lzfse)

set_target_properties(lzfse_cli PROPERTIES C_STANDARD 99 OUTPUT_NAME lzfse RUNTIME_OUTPUT_DIRECTORY_RELEASE "/bin")

#[[
if (BUILD_SHARED_LIBS)
	set_property(TARGET lzfse APPEND PROPERTY COMPILE_DEFINITIONS LZFSE_DLL LZFSE_DLL_EXPORTS)
	set_property(TARGET lzfse APPEND PROPERTY INTERFACE_COMPILE_DEFINITIONS LZFSE_DLL)
endif()
]]