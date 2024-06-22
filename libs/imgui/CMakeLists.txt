cmake_minimum_required(VERSION 3.21)

add_library(imgui STATIC
    imgui.cpp
	imgui_demo.cpp
	imgui_draw.cpp
	imgui_tables.cpp
	imgui_widgets.cpp 
	imgui_stdlib.cpp
)

target_include_directories(imgui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})