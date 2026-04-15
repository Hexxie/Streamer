#ifndef STREAMER_MP4_ENCODER_INPUT
#define STREAMER_MP4_ENCODER_INPUT

extern "C" {
  #include <libavcodec/avcodec.h>
  #include <libavcodec/codec_id.h>
}

#include <string>

namespace streamer {
  class Mp4Encoder {
  public:
    Mp4Encoder();
    AVCodecContext *getMp4EncoderContext() const;
    AVCodecID getEncoderId() const;

    int getWidth() const;
    int getHeight() const;
    AVPixelFormat getPixelFormat() const;

    void applySettings(int width, int height);
    void open();
  private:
  struct AVCodecDeleter {
        void operator()(AVCodecContext* ctx) const {
          if (ctx) {
            avcodec_free_context(&ctx);
          }
        }
    };
  std::unique_ptr<AVCodecContext, AVCodecDeleter> encoder_ctx;
  AVCodecID encoderId = AV_CODEC_ID_NONE;

  // This is just reference to particular codec
  // no need to free it
  const AVCodec *encoder = nullptr;
  };
}

#endif //STREAMER_MP4_ENCODER_INPUT