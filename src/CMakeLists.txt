file(GLOB SRC_FILES *.cpp)

add_library(streamer_lib ${SRC_FILES})

target_include_directories(streamer_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    /usr/local/include
)

target_link_directories(streamer_lib PUBLIC
    /usr/local/lib
)

target_link_libraries(streamer_lib PUBLIC
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