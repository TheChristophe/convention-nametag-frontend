#include "videoPlayer.hpp"
#include "videoDecoder.hpp"

VideoPlayer::VideoPlayer(int width, int height)
    : _width{ width }
    , _height{ height }
{
}

void VideoPlayer::FetchFrame(uint8_t *buffer, int bufferSize)
{
    _activeDecoder->DecodeFrame(buffer, bufferSize);
}

void VideoPlayer::PlayFile(const std::string &filename)
{
    // clean up previous player first
    // TODO: any ffmpeg components we can reuse?
    _activeDecoder.reset();
    _activeDecoder = std::make_unique<VideoDecoder>(filename, _width, _height);
}
