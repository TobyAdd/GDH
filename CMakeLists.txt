cmake_minimum_required(VERSION 3.0.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(PROJECT GDH)
project(${PROJECT})

add_compile_definitions(UNICODE _UNICODE)

file(GLOB_RECURSE SOURCE_FILES source/*.cpp)
add_library(${PROJECT} SHARED ${SOURCE_FILES})

target_include_directories(${PROJECT} PRIVATE
  libraries/minhook/include

  libraries/imgui
  libraries/imgui-hook
  libraries/imgui-notify
  
  libraries
)

add_subdirectory(libraries/minhook)
add_subdirectory(libraries/imgui-hook)

target_link_libraries(${PROJECT} minhook imgui-hook opengl32)

# add_custom_command(TARGET ${PROJECT} POST_BUILD
#                    COMMAND ${CMAKE_COMMAND} -E copy
#                        $<TARGET_FILE:${PROJECT}>
#                         D:/Games/GD22/GDH.dll)