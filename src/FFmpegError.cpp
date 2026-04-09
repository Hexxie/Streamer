#include "FFmpegError.hpp"

extern "C" {
  #include <libavutil/error.h>
}

streamer::FFmpegError::FFmpegError(std::string msg, int err_code)
  : err_code_(err_code)
{
  char buf[256];
  av_strerror(err_code, buf, sizeof(buf));
  message_ = msg + ": " + buf;
}

const char* streamer::FFmpegError::what() const noexcept {
  return message_.c_str();
}

int streamer::FFmpegError::code() const noexcept {
  return err_code_;
}