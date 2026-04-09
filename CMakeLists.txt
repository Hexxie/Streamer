cmake_minimum_required(VERSION 3.26)

project(Streamer)

find_library(FOUNDATION_FRAMEWORK Foundation)
find_library(AVFOUNDATION_FRAMEWORK AVFoundation)
find_library(COREMEDIA_FRAMEWORK CoreMedia)
find_library(COREVIDEO_FRAMEWORK CoreVideo)
find_library(APPKIT_FRAMEWORK AppKit)
find_library(VIDEOTOOLBOX_FRAMEWORK VideoToolbox)
find_library(AUDIOTOOLBOX_FRAMEWORK AudioToolbox)
find_library(COREAUDIO_FRAMEWORK CoreAudio)
find_library(SECURITY_FRAMEWORK Security)
find_library(COREIMAGE CoreImage)
find_library(OPENGL OpenGL)

add_subdirectory(src)

add_executable(streamer main.cpp)

target_include_directories(streamer PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(streamer PRIVATE streamer_lib)