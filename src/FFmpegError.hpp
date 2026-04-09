#ifndef STREAMER_FFMPEG_ERROR
#define STREAMER_FFMPEG_ERROR

#include <exception>
#include <string>

namespace streamer {

class FFmpegError : public std::exception {
public:
    FFmpegError(std::string msg, int err_code);
    const char* what() const noexcept override;
    int code() const noexcept;

private:
    std::string message_;
    int err_code_;
};

} 

#endif //STREAMER_FFMPEG_ERROR