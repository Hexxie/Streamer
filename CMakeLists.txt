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
    avdevice # Devices: camera, mic, screen
    avformat # IO: files, rtsp, http, rtp
    avfilter # Video filters: scale, blur, overlays
    avcodec # Codecs: H264, AAC, MJPEG, encode/decode
    avutil # Utils: logs, memory, structures
    swresample # Audio recsample - frequency, format
    swscale # Vide scake - RGB - YUV, resize
    z #zlib
    lzma # tiff, other codecs
    bz2 # bzip, could be in containers like matroska
    # macOS frameworks
    ${FOUNDATION_FRAMEWORK} # Objective-C runtime Apple API
    ${AVFOUNDATION_FRAMEWORK} # MacOS camera API
    ${COREMEDIA_FRAMEWORK} # Timings, buffers, mediastructures
    ${COREVIDEO_FRAMEWORK} # Work with video buffers
    ${APPKIT_FRAMEWORK} # UI
    ${VIDEOTOOLBOX_FRAMEWORK} # H264 hardware encode/decode
    ${AUDIOTOOLBOX_FRAMEWORK} # Low-level audio API
    ${COREAUDIO_FRAMEWORK} # System audio stack
    ${SECURITY_FRAMEWORK} # TLS, SSL, HTTPS, RTSP over TLS
    ${COREIMAGE} #avfilter's GPU filters
    ${OPENGL} #GPU rendering for coreimage and scaling
)