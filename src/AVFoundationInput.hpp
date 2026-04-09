#ifndef STREAMER_AVFOUNDATION_INPUT
#define STREAMER_AVFOUNDATION_INPUT

#include <string>

extern "C" {
  #include <libavdevice/avdevice.h>
}

namespace streamer {
  class AVFoundationInput {
  public:
    AVFoundationInput();
    ~AVFoundationInput();
    AVFormatContext *getContext() const;

    int getVideoStreamIdx() const;
    int getAudioStreamIdx() const;

    bool hasVideo() const;
    bool hasAudio() const;
  private:
    std::string name = "avfoundation";

    struct AVFormatContextDeleter {
        void operator()(AVFormatContext* ctx) const {
          if (ctx) {
            avformat_close_input(&ctx);
          }
        }
    };
    std::unique_ptr<AVFormatContext, AVFormatContextDeleter> input_ctx;
    bool videoIsFound = false;
    bool audioIsFound = false;
    int videoStreamIdx = -1;
    int audioStreamIdx = -1;

    void mapStreams();
  };
}

#endif //AVFOUNDATION_INPUT