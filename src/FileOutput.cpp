#include "FileOutput.hpp"
#include "FFmpegError.hpp"

streamer::FileOutput::FileOutput(std::string filename, std::string file_format) {
  AVFormatContext *raw_output_ctx = nullptr;
  int ret = avformat_alloc_output_context2(
    &raw_output_ctx, 
    nullptr, 
    file_format.c_str(), 
    filename.c_str());
  
  if (ret < 0 || !raw_output_ctx) {
    throw FFmpegError("Failed to create output context", ret);
  }

  this->filename = filename;
  output_ctx = std::unique_ptr<AVFormatContext, FileOutputDeleter> (raw_output_ctx);
}

AVFormatContext *streamer::FileOutput::getOutputContext() const {
  return output_ctx.get();
}

streamer::FileOutput::~FileOutput() {
  close();
}

// ToDo - accept options in paramether if you want
void streamer::FileOutput::open() {
  if (!(output_ctx->oformat->flags & AVFMT_NOFILE))
  {
    int ret = avio_open(&output_ctx->pb, filename.c_str(), AVIO_FLAG_WRITE);
    if (ret < 0)
    {
      throw FFmpegError("Failed to open output file:", ret);
    }

    ret = avformat_write_header(output_ctx.get(), nullptr);
    if (ret < 0)
    {
      throw FFmpegError("Failed to write mp4 header: ", ret);
      avio_closep(&output_ctx->pb);
    }
  }
}

void streamer::FileOutput::close() {
    if (!output_ctx) {
        return;
    }

    if (header_written && !trailer_written) {
        av_write_trailer(output_ctx.get());
        trailer_written = true;
    }

    if (!(output_ctx->oformat->flags & AVFMT_NOFILE) && output_ctx->pb) {
        avio_closep(&output_ctx->pb);
    }
}