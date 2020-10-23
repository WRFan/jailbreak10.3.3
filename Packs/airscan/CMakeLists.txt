project(airscan)

cmake_minimum_required(VERSION 3.14.2)

add_executable(airscan main.c)

# target_include_directories(airscan PUBLIC
	# /usr/include/iPhoneOS10.3_headers
# )

target_link_libraries(airscan PUBLIC
	/usr/include/iPhoneOS10.3.sdk/System/Library/PrivateFrameworks/MobileWiFi.framework
	/usr/include/iPhoneOS10.3.sdk/System/Library/Frameworks/CoreFoundation.framework/CoreFoundation.tbd
)

# set_target_properties(airscan PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE "/usr/bin")

add_custom_command(TARGET airscan POST_BUILD COMMAND jtool --sign --inplace --ent ${CMAKE_CURRENT_SOURCE_DIR}/entitlements.plist ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME} COMMAND mv ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME} "/usr/bin")