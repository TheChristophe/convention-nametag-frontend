#ifndef CONVENTION_NAMETAG_VIDEOPLAYER_HPP
#define CONVENTION_NAMETAG_VIDEOPLAYER_HPP

#include "decoder.hpp"
#include "idleDecoder.hpp"

#include <memory>

class VideoPlayer {
    public:
    VideoPlayer(int width, int height);
    ~VideoPlayer() = default;

    void FetchFrame(uint8_t *buffer, int bufferSize);
    void PlayFile(const std::string &filename);

    private:
    std::unique_ptr<Decoder> _activeDecoder{ std::make_unique<IdleDecoder>() };

    int _width;
    int _height;
};

#endif //CONVENTION_NAMETAG_VIDEOPLAYER_HPP
