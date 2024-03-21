cmake_minimum_required(VERSION 3.0)

add_library(imgui-hook STATIC
    imgui/imgui.cpp
	imgui/imgui_demo.cpp
	imgui/imgui_draw.cpp
	imgui/imgui_impl_opengl2.cpp
	imgui/imgui_impl_opengl3.cpp
	imgui/imgui_impl_win32.cpp
	imgui/imgui_tables.cpp
	imgui/imgui_widgets.cpp 
    imgui/imgui_stdlib.cpp
    imgui-hook.cpp
)

target_include_directories(imgui-hook PRIVATE imgui
    ../cocos-headers/cocos2dx/
    ../cocos-headers/cocos2dx/include
    ../cocos-headers/cocos2dx/kazmath/include
    ../cocos-headers/cocos2dx/platform/win32
    ../cocos-headers/cocos2dx/platform/third_party/win32
    ../cocos-headers/cocos2dx/platform/third_party/win32/OGLES
    ../stb
)