#include "videoPlayer.hpp"

#include <iostream>
#include <stdexcept>

VideoPlayer::VideoPlayer(const char *filename, int width, int height)
    : _outWidth{ width }
    , _outHeight{ height }
{
    if (avformat_open_input(&_formatContext, filename, nullptr, nullptr) != 0) {
        throw std::runtime_error("failed to open video file!");
    }

    av_dump_format(_formatContext, 0, filename, 0);

    for (int i = 0; i < static_cast<int>(_formatContext->nb_streams); i++) {
        if (_formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            _codecIndex      = i;
            _codecParameters = _formatContext->streams[i]->codecpar;
        }
    }
    if (_codecParameters == nullptr) {
        throw std::runtime_error("failed to find video stream!");
    }

    const AVCodec *codec = nullptr;
    // raspberry pi hardware accelerated
    if (_codecParameters->codec_id == AV_CODEC_ID_H264) {
        codec = avcodec_find_decoder_by_name("h264_omx");
    }

    // no specialized decoder, attempt automatic choice
    if (codec == nullptr) {
        codec = avcodec_find_decoder(_codecParameters->codec_id);

        // still nothing found
        if (codec == nullptr) {
            throw std::runtime_error("failed to find decoder!");
        }
    }

    _codecContext = avcodec_alloc_context3(codec);
    avcodec_parameters_to_context(_codecContext, _codecParameters);
    if (avcodec_open2(_codecContext, codec, nullptr) < 0) {
        throw std::runtime_error("failed to open codec!");
    }

    avformat_find_stream_info(_formatContext, nullptr);

    std::cout << _codecParameters << " " << _codecParameters->width << " " << _codecParameters->height << " " << _codecParameters->codec_id << " " << _codecParameters->format << std::endl;
    _swsContext = sws_getContext(_codecParameters->width, _codecParameters->height,
        static_cast<AVPixelFormat>(_codecParameters->format), _outWidth, _outHeight, AV_PIX_FMT_RGB24,
        SWS_BILINEAR, nullptr, nullptr, nullptr);
}

VideoPlayer::~VideoPlayer()
{
    //avcodec_close(_codecContext);
    avformat_close_input(&_formatContext);
}

void VideoPlayer::GetFrame(uint8_t *outBuffer, size_t bufferSize)
{
    AVFrame *frame    = av_frame_alloc();
    AVFrame *frameRGB = av_frame_alloc();

    const auto requiredBufferSize = av_image_get_buffer_size(AV_PIX_FMT_RGB24, 256, 64, 1);
    av_image_alloc(frameRGB->data, frameRGB->linesize, _outWidth, _outHeight, AV_PIX_FMT_RGB24, 1);

    // TODO: ffmpeg buffer size calculation using AV_PIX_FMT_RGB24

    AVPacket packet;

    static int i = 0;
    while (1) {
        int r = av_read_frame(_formatContext, &packet);
        if (r < 0) {
            std::cout << "Error or EOF " << r << std::endl;
            std::exit(0);
        }
        if (packet.stream_index == _codecIndex) {
            int ret = avcodec_send_packet(_codecContext, &packet);
            if (ret != 0) {
                std::cout << "packet " << ret << std::endl;
            }
            if (ret == AVERROR(EAGAIN)) {
                throw std::runtime_error("EAGAIN");
            }
            if (ret == AVERROR_EOF) {
                std::cout << "packet EOF" << std::endl;
                std::exit(0);
                return;
            }
            if (ret == AVERROR(EINVAL)) {
                throw std::runtime_error("EINVAL");
            }
            if (ret == AVERROR(ENOMEM)) {
                throw std::runtime_error("Shit's fucked");
            }
            if (ret < 0) {
                std::cout << "Unknown packet error" << std::endl;
                std::exit(0);
            }
            while (ret >= 0) {
                ret = avcodec_receive_frame(_codecContext, frame);
                if (ret != 0) {
                    std::cout << "frame " << ret << std::endl;
                }
                if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                    std::cout << "break" << std::endl;
                    break;
                }
                if (ret == AVERROR(EINVAL)) {
                    throw std::runtime_error("Shit's even more fucked");
                }
                if (ret < 0) {
                    std::cout << "Unknown frame error" << std::endl;
                }

                sws_scale(_swsContext, frame->data, frame->linesize, 0, _codecContext->height, frameRGB->data, frameRGB->linesize);
                memcpy(outBuffer, frameRGB->data[0], bufferSize);
                av_packet_unref(&packet);
                return;
            }
        }
    }
}
