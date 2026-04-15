#ifndef STREAMER_VIDEO_FRAME_CONVERTER
#define STREAMER_VIDEO_FRAME_CONVERTER

#include <string>

#include "VideoDecoder.hpp"
#include "Mp4Encoder.hpp"

extern "C" {
  #include <libswscale/swscale.h>
}

namespace streamer {
  class VideoFrameConverter {
  public:
    VideoFrameConverter(streamer::VideoDecoder& decoder, streamer::Mp4Encoder& encoder);
    SwsContext *getSwsContext() const;
  private:
  struct SwsContextDeleter {
        void operator()(SwsContext* ctx) const {
          if (ctx) {
            sws_freeContext(ctx);
          }
        }
    };

    std::unique_ptr<SwsContext, SwsContextDeleter> sws_ctx;

  };
}

#endif //STREAMER_VIDEO_FRAME_CONVERTER