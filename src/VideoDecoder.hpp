#ifndef STREAMER_VIDEO_DECODER_INPUT
#define STREAMER_VIDEO_DECODER_INPUT

#include <string>

#include "AVFoundationInput.hpp"

extern "C" {
  #include <libavcodec/avcodec.h>
}

namespace streamer {
  class VideoDecoder {
  public:
    VideoDecoder(streamer::AVFoundationInput &input, int stream_idx);
    ~VideoDecoder();
    AVCodecContext *getDecoderContext() const;
  private:
    struct AVCodecContextDeleter {
        void operator()(AVCodecContext* ctx) const {
          if (ctx) {
            avcodec_free_context(&ctx);
          }
        }
    };

    std::unique_ptr<AVCodecContext, AVCodecContextDeleter> decoder_ctx;
  };
}

#endif //STREAMER_VIDEO_DECODER_INPUT