#include "AVFoundationInput.hpp"
#include "FFmpegError.hpp"

streamer::AVFoundationInput::AVFoundationInput() {
  avdevice_register_all();
  const AVInputFormat *input_fmt = av_find_input_format("avfoundation");
  if (!input_fmt){
    throw std::runtime_error("Could not find avfoundation input format");
  }

  // Actually think about other constructor to be able to fill
  // this things
  AVDictionary *options = nullptr;
  av_dict_set(&options, "framerate", "30", 0);
  av_dict_set(&options, "video_size", "640x480", 0);
  av_dict_set(&options, "pixel_format", "0rgb", 0);

  // TBD: Maybe it should be another class which would register
  // any input device
  AVFormatContext* raw_ctx = nullptr;
  int ret = avformat_open_input(&raw_ctx, "0:none", input_fmt, &options);
  av_dict_free(&options);
  if (ret < 0) {
    throw FFmpegError("Failed to open camera", ret);
  }

  input_ctx = std::unique_ptr<AVFormatContext, AVFormatContextDeleter>(raw_ctx);
  mapStreams();
}

streamer::AVFoundationInput::~AVFoundationInput() {
}

AVFormatContext *streamer::AVFoundationInput::getContext() const {
  return input_ctx.get();
}

int streamer::AVFoundationInput::getVideoStreamIdx() const {
  return videoStreamIdx;
}

int streamer::AVFoundationInput::getAudioStreamIdx() const {
  return audioStreamIdx;
}

bool streamer::AVFoundationInput::hasVideo() const {
  return videoIsFound;
}

bool streamer::AVFoundationInput::hasAudio() const {
  return audioIsFound;
}

void streamer::AVFoundationInput::mapStreams() {
  int ret = avformat_find_stream_info(input_ctx.get(), nullptr);
  if (ret < 0) {
    throw FFmpegError("Failed to find stream info:", ret);
  }

  // TODO: This is dangerous. What if we have few video streams!
  for (int i = 0; i < input_ctx->nb_streams; i++)
  {
    if (input_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
    {
      videoIsFound = true;
      videoStreamIdx = i;
    }
    if (input_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
    {
      audioIsFound = true;
      audioStreamIdx = i;
    }
  }
}