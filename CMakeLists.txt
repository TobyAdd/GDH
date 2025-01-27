cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

project(GDH VERSION 1.0.0)

file(GLOB_RECURSE SOURCES src/*.cpp)
add_library(${PROJECT_NAME} SHARED ${SOURCES})

if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR "Unable to find Geode SDK! Please define GEODE_SDK environment variable to point to Geode")
else()
    message(STATUS "Found Geode: $ENV{GEODE_SDK}")
endif()

add_subdirectory($ENV{GEODE_SDK} ${CMAKE_CURRENT_BINARY_DIR}/geode)

target_include_directories(${PROJECT_NAME} PRIVATE libs libs/ffmpeg)

set(HAS_IMGUI ON)
add_subdirectory(libs/imgui)
CPMAddPackage("gh:matcool/gd-imgui-cocos#8973953")

target_link_libraries(${PROJECT_NAME} imgui-cocos imgui)

if (ANDROID) 
    target_link_libraries(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/libs/ffmpeg/libavcodec/libavcodec.a)
    target_link_libraries(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/libs/ffmpeg/libavdevice/libavdevice.a)
    target_link_libraries(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/libs/ffmpeg/libavfilter/libavfilter.a)
    target_link_libraries(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/libs/ffmpeg/libavformat/libavformat.a)
    target_link_libraries(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/libs/ffmpeg/libavutil/libavutil.a)
    target_link_libraries(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/libs/ffmpeg/libpostproc/libpostproc.a)
    target_link_libraries(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/libs/ffmpeg/libswresample/libswresample.a)
    target_link_libraries(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/libs/ffmpeg/libswscale/libswscale.a)
    target_link_libraries(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/libs/ffmpeg/x264/libx264.a)
endif()

setup_geode_mod(${PROJECT_NAME})
