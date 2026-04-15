#include "Mp4Encoder.hpp"
#include "FFmpegError.hpp"

#include <string>

streamer::Mp4Encoder::Mp4Encoder(){
  encoder = avcodec_find_encoder_by_name("h264_videotoolbox");

  if(!encoder) {
    throw std::runtime_error("Encoder h264_videotoolbox not found");
  }

  encoderId = encoder->id;

  AVCodecContext *raw_encoder_ctx = avcodec_alloc_context3(encoder);
  if(!raw_encoder_ctx) {
    throw std::runtime_error("Could not allocate Mp4Encoder context");
  }

  encoder_ctx = std::unique_ptr<AVCodecContext, AVCodecDeleter> (raw_encoder_ctx);
}

AVCodecContext *streamer::Mp4Encoder::getMp4EncoderContext() const {
  return encoder_ctx.get();
}

enum AVCodecID streamer::Mp4Encoder::getEncoderId() const {
  return encoderId;
}

void streamer::Mp4Encoder::applySettings(int width, int height) {
  encoder_ctx->codec_id = encoderId;
  encoder_ctx->codec_type = AVMEDIA_TYPE_VIDEO;
  encoder_ctx->width = width;
  encoder_ctx->height = height;
  encoder_ctx->time_base = AVRational{1, 30};
  encoder_ctx->framerate = AVRational{30, 1};
  encoder_ctx->pix_fmt = AV_PIX_FMT_NV12;
  encoder_ctx->gop_size = 12;
  encoder_ctx->max_b_frames = 0;
  encoder_ctx->bit_rate = 2'000'000;
  encoder_ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
}
void streamer::Mp4Encoder::open() {
  int ret = avcodec_open2(encoder_ctx.get(), encoder, nullptr);
  if (ret < 0) {
      throw FFmpegError("Failed to open encoder:", ret);
  }
}

int streamer::Mp4Encoder::getWidth() const {
  return encoder_ctx->width;
}

int streamer::Mp4Encoder::getHeight() const {
  return encoder_ctx->height;
}

AVPixelFormat streamer::Mp4Encoder::getPixelFormat() const {
  return encoder_ctx->pix_fmt;
}