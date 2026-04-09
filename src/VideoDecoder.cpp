#include "VideoDecoder.hpp"
#include "FFmpegError.hpp"

streamer::VideoDecoder::VideoDecoder( streamer::AVFoundationInput &input, int stream_idx) {
  // To do - think about better way to use context. We've already passed C++ object here
  AVStream *in_stream = input.getContext()->streams[input.getVideoStreamIdx()];

  const AVCodec *decoder = avcodec_find_decoder(in_stream->codecpar->codec_id);
  if(!decoder) {
    throw std::runtime_error("Decoder not found");
  }

  decoder_ctx = std::unique_ptr<AVCodecContext, streamer::VideoDecoder::AVCodecContextDeleter>(
        avcodec_alloc_context3(decoder)
    );
  if(!decoder_ctx) {
    throw std::runtime_error("Could not allocate decoder context");
  }

  //Copy paramethers from stream to decoder
  int ret = avcodec_parameters_to_context(decoder_ctx.get(), in_stream->codecpar);
  if (ret < 0) {
      throw FFmpegError("Failed to copy decoder parameters:", ret);
  }

  // Finish with decoder initialization
  ret = avcodec_open2(decoder_ctx.get(), decoder, nullptr);
  if (ret < 0) {
      throw FFmpegError("Failed to open decoder:", ret);
  }
}

streamer::VideoDecoder::~VideoDecoder() {
}

AVCodecContext *streamer::VideoDecoder::getDecoderContext() const{
  return decoder_ctx.get();
}