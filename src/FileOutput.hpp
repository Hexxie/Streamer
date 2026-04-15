#ifndef STREAMER_FILE_OUTPUT
#define STREAMER_FILE_OUTPUT

#include <string>

extern "C" {
  #include <libavformat/avformat.h>
}

namespace streamer {
  class FileOutput {
  public:
    FileOutput(std::string filename, std::string file_format);
    ~FileOutput();
    AVFormatContext *getOutputContext() const;

    void open();
    void close();
  private:
  struct FileOutputDeleter {
        void operator()(AVFormatContext* ctx) const {
          if (ctx) {
            avformat_free_context(ctx);
          }
        }
    };

    std::unique_ptr<AVFormatContext, FileOutputDeleter> output_ctx;
    std::string filename = "unknown.mp4";
    bool header_written = false;
    bool trailer_written = false;
  };
}

#endif //STREAMER_FILE_OUTPUT