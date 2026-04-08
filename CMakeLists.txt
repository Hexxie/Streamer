cmake_minimum_required(VERSION 3.26)

project(Streamer)

add_executable(streamer main.cpp)

target_include_directories(streamer PUBLIC
    /usr/local/include
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_directories(streamer PUBLIC
    /usr/local/lib
)

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

target_link_libraries(streamer PUBLIC 
    avdevice 
    avformat
    avfilter
    avcodec 
    avutil
    swresample
    swscale
    z
    lzma
    bz2
    ${FOUNDATION_FRAMEWORK}
    ${AVFOUNDATION_FRAMEWORK}
    ${COREMEDIA_FRAMEWORK}
    ${COREVIDEO_FRAMEWORK}
    ${APPKIT_FRAMEWORK}
    ${VIDEOTOOLBOX_FRAMEWORK}
    ${AUDIOTOOLBOX_FRAMEWORK}
    ${COREAUDIO_FRAMEWORK}
    ${SECURITY_FRAMEWORK}
    ${COREIMAGE}
    ${OPENGL}
)