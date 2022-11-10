cmake_minimum_required(VERSION 3.3.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
cmake_policy(SET CMP0057 NEW)

project(offset-test)
add_executable(offset-test generated.cpp)
target_include_directories(
  offset-test PRIVATE
  cocos-headers/
  cocos-headers/extensions
  cocos-headers/cocos2dx
  cocos-headers/cocos2dx/include
  cocos-headers/cocos2dx/kazmath/include
  cocos-headers/cocos2dx/platform/win32
  cocos-headers/cocos2dx/platform/third_party/win32
  cocos-headers/cocos2dx/platform/third_party/win32/OGLES
  ../include
  ../
)
target_link_libraries(
    offset-test
    ${CMAKE_SOURCE_DIR}/cocos-headers/cocos2dx/libcocos2d.lib
    ${CMAKE_SOURCE_DIR}/cocos-headers/extensions/libExtensions.lib
)