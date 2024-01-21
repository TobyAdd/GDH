cmake_minimum_required(VERSION 3.0.0)
set(CMAKE_CXX_STANDARD 17)

set(PROJECT GDH)
project(${PROJECT})

add_compile_definitions(UNICODE _UNICODE)

file(GLOB_RECURSE SOURCE_FILES source/*.cpp)
add_library(${PROJECT} SHARED ${SOURCE_FILES})

target_include_directories(${PROJECT} PRIVATE
    libraries/minhook/include
    libraries/imgui-hook
    libraries/imgui-hook/imgui
    libraries/cocos-headers/extensions
    libraries/cocos-headers/cocos2dx/platform/third_party/win32
    libraries/cocos-headers/cocos2dx/platform/third_party/win32/OGLES
    libraries/cocos-headers/cocos2dx/platform/win32
    libraries/cocos-headers/cocos2dx/kazmath/include
    libraries/cocos-headers/cocos2dx/include
    libraries/cocos-headers/cocos2dx
    libraries
)

add_subdirectory(libraries/minhook)
add_subdirectory(libraries/imgui-hook)
add_subdirectory(libraries/cocos-headers)

target_link_libraries(${PROJECT} minhook imgui-hook opengl32 cocos2d)

add_definitions(-D_CRT_SECURE_NO_WARNINGS)

# # Specify the output directory for the DLL
# set_target_properties(${PROJECT} PROPERTIES
#     LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin
#     RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin
# )

# # Set the desired folder to copy the DLL after build
# set(DESTINATION_FOLDER "D:/Games/GD2204")

# # Copy the DLL to the desired folder after build
# add_custom_command(TARGET ${PROJECT} POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E copy
#     $<TARGET_FILE:${PROJECT}>
#     ${DESTINATION_FOLDER}
# )
