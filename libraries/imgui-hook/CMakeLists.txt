cmake_minimum_required(VERSION 3.0)

add_library(imgui-hook STATIC
    imgui/imgui.cpp
	imgui/imgui_demo.cpp
	imgui/imgui_draw.cpp
	imgui/imgui_impl_opengl3.cpp
	imgui/imgui_impl_win32.cpp
	imgui/imgui_tables.cpp
	imgui/imgui_widgets.cpp 
    imgui-hook.cpp
)