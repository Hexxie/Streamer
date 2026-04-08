#include <iostream>

extern "C" {
  #include <libavdevice/avdevice.h>
  #include <libavformat/avformat.h>
  #include <libavutil/dict.h>
  #include <libavutil/error.h>
}

int main() {
  avdevice_register_all();

  // Demuxers read a media file and split it into chunks of data
  // https://ffmpeg.org/doxygen/6.1/group__lavf__decoding.html
  const AVInputFormat* input = av_find_input_format("avfoundation");

  if(!input) {
    std::cerr << "Could not find avfoundation" << std::endl;
    return 1;
  }

  AVFormatContext *ctx = nullptr;
  AVDictionary *options = nullptr;

  av_dict_set(&options, "framerate", "30", 0);
  av_dict_set(&options, "video_size", "640x480", 0);
  av_dict_set(&options, "pixel_format", "0rgb", 0);

  int ret = avformat_open_input(&ctx, "0:none", input, &options);
  av_dict_free(&options);

  if (ret < 0) {
    std::cerr << "Failed to open camera: ";
    char buf[256];
    av_strerror(ret, buf, sizeof(buf));
    std::cerr << buf << std::endl;
    return 1;
  }

  std::cout << "Camera opened successfully" << std::endl;
  avformat_close_input(&ctx);
  return 0;
}