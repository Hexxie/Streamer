# Streamer

This is a playground app to play around with vide streaming libs using my macs camera.

## Goals: 
- Investigate RTSP protocol
- RTP/RTCP
- Videocodecs HEVC, H.264
- GRPC/web sockets, WebRTC, GST, LiveMedia, Video codecs/playback

Here I use libs from ffmpeg. And possibly OpenCV. 


Note for me about ffmpeg:

libavformat/	-> demux/mux (RTSP is here!)
libavcodec/ ->	encode/decode
libavdevice/	-> camera (avfoundation here)
libavutil/	-> utils