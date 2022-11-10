cmake_minimum_required(VERSION 3.0.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(GDH)

file(GLOB_RECURSE SOURCES 
    src/*.cpp
)

add_library(GDH SHARED ${SOURCES})

target_include_directories(GDH PRIVATE
  libraries/minhook/include
  libraries/gd.h/include
  libraries/gd.h/
  libraries/cocos-headers/cocos2dx/
  libraries/cocos-headers/cocos2dx/include
  libraries/cocos-headers/cocos2dx/kazmath/include
  libraries/cocos-headers/cocos2dx/platform/win32
  libraries/cocos-headers/cocos2dx/platform/third_party/win32
  libraries/cocos-headers/cocos2dx/platform/third_party/win32/OGLES
  libraries/cocos-headers/extensions/
  libraries/imgui-hook
  libraries/imgui-hook/imgui
)

add_subdirectory(libraries/minhook)
add_subdirectory(libraries/imgui-hook)
target_link_libraries(GDH 
  ${CMAKE_SOURCE_DIR}/libraries/cocos-headers/cocos2dx/libcocos2d.lib
  imgui-hook
  minhook
  opengl32
)