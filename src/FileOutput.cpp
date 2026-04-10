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

  output_ctx = std::unique_ptr<AVFormatContext, FileOutputDeleter> (raw_output_ctx);
}

AVFormatContext *streamer::FileOutput::getOutputContext() const {
  return output_ctx.get();
}