# Streamer

This is a playground app to play around with video streaming libs using my macs camera.

## Goals: 
- Investigate RTSP protocol
- RTP/RTCP
- Videocodecs HEVC, H.264
- GRPC/web sockets, WebRTC, GST, LiveMedia, Video codecs/playback

Here I use libs from ffmpeg. And possibly OpenCV. 

## How to build

1. First install libraries for ffmpeg
```
brew install automake fdk-aac git lame libass libtool libvorbis libvpx \
opus sdl shtool texi2html theora wget x264 x265 xvid nasm
```

2. Download ffmpeg source code from github
```
git clone https://github.com/FFmpeg/FFmpeg.git
```

3. Now you can build ffmpeg and libs
```
./configure  --prefix=/usr/local --enable-gpl --enable-nonfree
make
make install
```

4. Your libraries would be in /usr/local/lib and headers in /usr/local/include

5. To build the project create a build folder and make it
```
mkdir build
cd build
cmake ..
make
```

## Setup you vscode to navigate ffmpeg source code
```
touch .vscode/c_cpp_proprties.json
```

Fill the file with the script

```
{
  "configurations": [
    {
      "name": "Mac",
      "compilerPath": "/usr/bin/clang++",
      "cppStandard": "c++17",
      "includePath": [
        "${workspaceFolder}/**",
        "/usr/local/include",
        "/Users/path_to_project/FFmpeg"
      ],
      "browse": {
        "path": [
          "/usr/local/include",
          "/Users/path_to_project/FFmpeg"
        ]
      }
    }
  ],
  "version": 4
}
```


### Note for me about ffmpeg:

libavformat/	-> demux/mux (RTSP is here!)

libavcodec/ ->	encode/decode

libavdevice/	-> camera (avfoundation here)

libavutil/	-> utils
