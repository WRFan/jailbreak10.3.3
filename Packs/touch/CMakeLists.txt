project(touch C)

cmake_minimum_required(VERSION 3.14.2)

add_executable(touch touch.c)

set_target_properties(touch PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE "/bin")