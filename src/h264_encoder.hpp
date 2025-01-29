#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <cstring>
#include <stdexcept>
#include <memory>

extern "C" {
    #include <libavcodec/avcodec.h>
    #include <libavutil/opt.h>
    #include <libavutil/imgutils.h>
    #include <libavformat/avformat.h>
    #include <libswscale/swscale.h>
}

#include "config.hpp"
inline void logMessage(const std::string& message) {
    std::ofstream logFile(folderPath / "h264_encode.txt", std::ios::app);
    if (logFile.is_open()) {
        logFile << message << std::endl;
        logFile.close();
    }
}

class H264Encoder {
public:
    H264Encoder(int width, int height, int fps, int64_t bitrate, const std::filesystem::path& outputFilePath) 
        : m_frameCount(0) {
        if (width <= 0 || height <= 0 || fps <= 0 || bitrate <= 0) {
            logMessage("Invalid encoder parameters");
        }

        initializeEncoder(width, height, fps, bitrate, outputFilePath);
    }

    H264Encoder(const H264Encoder&) = delete;
    H264Encoder& operator=(const H264Encoder&) = delete;

    H264Encoder(H264Encoder&&) noexcept = default;
    H264Encoder& operator=(H264Encoder&&) noexcept = default;

    ~H264Encoder() {
        cleanup();
    }

    int finish() {
        logMessage("Finishing encoding process...");
        
        try {
            int result = encode_video_frame(c.get(), nullptr, pkt.get(), fmt_ctx.get());
            if (result < 0) {
                logMessage("Failed to encode final frame");
            }

            av_write_trailer(fmt_ctx.get());
            return result;
        }
        catch (const std::exception& e) {
            logMessage("Error during finish: " + std::string(e.what()));
            return -1;
        }
    }

    bool writeFrame(const std::vector<uint8_t>& frameData) {
        if (frameData.empty()) {
            logMessage("Empty frame data provided");
        }

        const size_t expectedSize = c->width * c->height * 3;
        if (frameData.size() != expectedSize) {
            logMessage("Invalid frame data size");
        }

        try {
            return writeFrameInternal(frameData);
        }
        catch (const std::exception& e) {
            logMessage("Error writing frame: " + std::string(e.what()));
            return false;
        }
    }

private:
    int m_frameCount;
    
    struct AVCodecContextDeleter {
        void operator()(AVCodecContext* p) { avcodec_free_context(&p); }
    };
    struct AVFormatContextDeleter {
        void operator()(AVFormatContext* p) { avformat_free_context(p); }
    };
    struct AVFrameDeleter {
        void operator()(AVFrame* p) { av_frame_free(&p); }
    };
    struct AVPacketDeleter {
        void operator()(AVPacket* p) { av_packet_free(&p); }
    };

    std::unique_ptr<AVCodecContext, AVCodecContextDeleter> c;
    std::unique_ptr<AVFormatContext, AVFormatContextDeleter> fmt_ctx;
    std::unique_ptr<AVFrame, AVFrameDeleter> frame;
    std::unique_ptr<AVPacket, AVPacketDeleter> pkt;

    void initializeEncoder(int width, int height, int fps, int64_t bitrate, 
                         const std::filesystem::path& outputFilePath) {
        logMessage("== Start of encoding ==");
        logMessage("Initializing H264Encoder with parameters:");
        logMessage("Width: " + std::to_string(width));
        logMessage("Height: " + std::to_string(height));
        logMessage("FPS: " + std::to_string(fps));
        logMessage("Bitrate: " + std::to_string(bitrate));
        logMessage("Output file: " + outputFilePath.string());

        const AVCodec* codec = avcodec_find_encoder_by_name("libx264");
        if (!codec) {
            logMessage("Could not find libx264 codec");
        }

        AVCodecContext* codec_ctx = avcodec_alloc_context3(codec);
        if (!codec_ctx) {
            logMessage("Could not allocate codec context");
        }
        c.reset(codec_ctx);

        c->bit_rate = std::max(int64_t(0), std::min(bitrate, int64_t(INT_MAX)));
        c->width = width;
        c->height = height;
        c->time_base = (AVRational){1, fps};
        c->framerate = (AVRational){fps, 1};
        c->gop_size = 10;
        c->max_b_frames = 1;
        c->pix_fmt = AV_PIX_FMT_YUV420P;

        if (width % 2 != 0 || height % 2 != 0) {
            logMessage("Width and height must be even numbers");
        }

        if (av_opt_set(c->priv_data, "preset", "medium", 0) < 0) {
            logMessage("Failed to set encoder preset");
        }

        setupFormatContext(outputFilePath, codec);
        setupFrameAndPacket();
    }

    void setupFormatContext(const std::filesystem::path& outputFilePath, const AVCodec* codec) {
        AVFormatContext* format_ctx = nullptr;
        std::string filenameStr = outputFilePath.string();
        
        if (avformat_alloc_output_context2(&format_ctx, nullptr, nullptr, 
                                         filenameStr.c_str()) < 0) {
            logMessage("Could not allocate output context");
        }
        fmt_ctx.reset(format_ctx);

        AVStream* stream = avformat_new_stream(fmt_ctx.get(), codec);
        if (!stream) {
            logMessage("Could not create new stream");
        }

        if (avcodec_open2(c.get(), codec, nullptr) < 0) {
            logMessage("Could not open codec");
        }

        if (avcodec_parameters_from_context(stream->codecpar, c.get()) < 0) {
            logMessage("Could not copy codec parameters");
        }

        if (avio_open(&fmt_ctx->pb, filenameStr.c_str(), AVIO_FLAG_WRITE) < 0) {
            logMessage("Could not open output file");
        }

        if (avformat_write_header(fmt_ctx.get(), nullptr) < 0) {
            logMessage("Could not write header");
        }
    }

    void setupFrameAndPacket() {
        AVFrame* frame_ptr = av_frame_alloc();
        if (!frame_ptr) {
            logMessage("Could not allocate video frame");
        }
        frame.reset(frame_ptr);

        frame->format = c->pix_fmt;
        frame->width = c->width;
        frame->height = c->height;

        if (av_frame_get_buffer(frame.get(), 0) < 0) {
            logMessage("Could not allocate frame buffer");
        }

        AVPacket* pkt_ptr = av_packet_alloc();
        if (!pkt_ptr) {
            logMessage("Could not allocate packet");
        }
        pkt.reset(pkt_ptr);
    }

    bool writeFrameInternal(const std::vector<uint8_t>& frameData) {
        if (av_frame_make_writable(frame.get()) < 0) {
            logMessage("Could not make frame writable");
            return false;
        }

        std::unique_ptr<AVFrame, AVFrameDeleter> tempFrame(av_frame_alloc());
        if (!tempFrame) {
            logMessage("Could not allocate temporary frame");
            return false;
        }

        tempFrame->format = AV_PIX_FMT_RGB24;
        tempFrame->width = c->width;
        tempFrame->height = c->height;

        if (av_frame_get_buffer(tempFrame.get(), 0) < 0) {
            logMessage("Could not allocate temporary frame buffer");
            return false;
        }

        std::memcpy(tempFrame->data[0], frameData.data(), frameData.size());

        struct SwsContextDeleter {
            void operator()(SwsContext* p) { sws_freeContext(p); }
        };

        std::unique_ptr<SwsContext, SwsContextDeleter> swsCtx(
            sws_getContext(c->width, c->height, AV_PIX_FMT_RGB24,
                          c->width, c->height, AV_PIX_FMT_YUV420P,
                          0, nullptr, nullptr, nullptr)
        );

        if (!swsCtx) {
            logMessage("Could not create SwsContext");
            return false;
        }

        if (sws_scale(swsCtx.get(), tempFrame->data, tempFrame->linesize, 0, c->height, frame->data, frame->linesize) < 0) {
            logMessage("Could not scale frame");
            return false;
        }

        frame->pts = m_frameCount++;
        
        if (encode_video_frame(c.get(), frame.get(), pkt.get(), fmt_ctx.get()) < 0) {
            logMessage("Could not encode video frame");
            return false;
        }

        return true;
    }

    int encode_video_frame(AVCodecContext *enc_ctx, AVFrame *frame, AVPacket *pkt, AVFormatContext *fmt_ctx) {
        int ret = avcodec_send_frame(enc_ctx, frame);
        if (ret < 0) {
            logMessage("Could not send frame to encoder");
            return false;
        }

        while (ret >= 0) {
            ret = avcodec_receive_packet(enc_ctx, pkt);
            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                return 0;
            }
            if (ret < 0) {
                logMessage("Could not receive packet from encoder");
                return false;
            }

            av_packet_rescale_ts(pkt, enc_ctx->time_base, fmt_ctx->streams[0]->time_base);
            pkt->stream_index = 0;

            ret = av_interleaved_write_frame(fmt_ctx, pkt);
            if (ret < 0) {
                logMessage("Could not write packet to file");
                return false;
            }
            av_packet_unref(pkt);
        }

        return 0;
    }

    void cleanup() {
        if (fmt_ctx && fmt_ctx->pb) {
            avio_closep(&fmt_ctx->pb);
        }
        logMessage("== End of encoding ==");
    }
};