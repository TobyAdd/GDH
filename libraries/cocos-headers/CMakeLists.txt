cmake_minimum_required(VERSION 3.0.0...3.29.0)

project(cocos-headers CXX)

add_library(cocos2d INTERFACE)

target_include_directories(cocos2d INTERFACE 
  cocos2dx
  cocos2dx/include
  cocos2dx/kazmath/include
  cocos2dx/platform/win32
  cocos2dx/platform/third_party/win32
  cocos2dx/platform/third_party/win32/OGLES
  extensions
)

if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    target_link_libraries(cocos2d INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/cocos2dx/libcocos2d.lib
        ${CMAKE_CURRENT_LIST_DIR}/extensions/libExtensions.lib
    )
else()
    target_link_libraries(cocos2d INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/cocos2dx/libcocos2d32.lib
        ${CMAKE_CURRENT_LIST_DIR}/extensions/libExtensions32.lib
    )
endif()


