#ifndef CONVENTION_NAMETAG_VIDEOPLAYER_HPP
#define CONVENTION_NAMETAG_VIDEOPLAYER_HPP

#include "player.hpp"

// extern C required
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}

#include <chrono>

class VideoPlayer : public Player {
    public:
    explicit VideoPlayer(const char *filename, int width, int height);
    ~VideoPlayer() override;

    void GetFrame(uint8_t *buffer, size_t bufferSize) override;

    private:
    AVFormatContext *_formatContext{};
    int _codecIndex;
    AVCodecParameters *_codecParameters;
    AVCodecContext *_codecContext;
    struct SwsContext *_swsContext;
    AVStream *_videoStream;

    AVFrame *_rgbFrameBuffer{ av_frame_alloc() };

    const int _outWidth;
    const int _outHeight;

    std::chrono::time_point<std::chrono::steady_clock> _startTime;
};

#endif
