#include "VideoFrameConverter.hpp"
#include "FFmpegError.hpp"

// And here of course I want to see interfaces
// But we'll start with concrete classes by references
streamer::VideoFrameConverter::VideoFrameConverter(
  streamer::VideoDecoder& decoder, 
  streamer::Mp4Encoder& encoder){
    SwsContext *raw_sws_ctx = sws_getContext(
      decoder.getWidth(),
      decoder.getHeight(),
      decoder.getPixelFormat(),
      encoder.getWidth(),
      encoder.getHeight(),
      encoder.getPixelFormat(),
      SWS_BILINEAR,
      nullptr,
      nullptr,
      nullptr);

    if (!raw_sws_ctx)
    {
      throw std::runtime_error("VideoFrameConverter not found");
    }

      sws_ctx = std::unique_ptr<SwsContext, SwsContextDeleter> (raw_sws_ctx);
}

SwsContext *streamer::VideoFrameConverter::getSwsContext() const {
  return sws_ctx.get();
}