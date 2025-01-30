#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <cstring>
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
    }
    logFile.close();
}

class H264Encoder {
public:
    H264Encoder(int width, int height, int fps, int64_t bitrate, const std::filesystem::path& outputFilePath) 
        : m_frameCount(0) {
        if (width <= 0 || height <= 0 || fps <= 0 || bitrate <= 0) {
            logMessage("Error: Invalid encoder parameters (width, height, fps, bitrate must be positive)");
            return;
        }
        if (width % 2 != 0 || height % 2 != 0) {
            logMessage("Error: Width and height must be even numbers");
            return;
        }

        if (!initializeEncoder(width, height, fps, bitrate, outputFilePath)) {
            logMessage("Error: Failed to initialize encoder");
        }
    }

    H264Encoder(const H264Encoder&) = delete;
    H264Encoder& operator=(const H264Encoder&) = delete;

    ~H264Encoder() {
        cleanup();
    }

    int finish() {
        logMessage("Finishing encoding process...");
        
        int result = encode_video_frame(c.get(), nullptr, pkt.get(), fmt_ctx.get());
        if (result < 0) {
            logMessage("Error: Failed to encode final frame");
        }

        if (fmt_ctx) {
            av_write_trailer(fmt_ctx.get());
        }

        return result;
    }

    bool writeFrame(const std::vector<uint8_t>& frameData) {
        if (frameData.empty()) {
            logMessage("Error: Empty frame data provided");
            return false;
        }

        const size_t expectedSize = c->width * c->height * 3;
        if (frameData.size() != expectedSize) {
            logMessage("Error: Invalid frame data size. Expected " + 
                      std::to_string(expectedSize) + ", got " + 
                      std::to_string(frameData.size()));
            return false;
        }

        return writeFrameInternal(frameData);
    }

private:
    int m_frameCount;
    
    struct AVCodecContextDeleter { void operator()(AVCodecContext* p) { avcodec_free_context(&p); } };
    struct AVFormatContextDeleter { void operator()(AVFormatContext* p) { avformat_free_context(p); } };
    struct AVFrameDeleter { void operator()(AVFrame* p) { av_frame_free(&p); } };
    struct AVPacketDeleter { void operator()(AVPacket* p) { av_packet_free(&p); } };

    std::unique_ptr<AVCodecContext, AVCodecContextDeleter> c;
    std::unique_ptr<AVFormatContext, AVFormatContextDeleter> fmt_ctx;
    std::unique_ptr<AVFrame, AVFrameDeleter> frame;
    std::unique_ptr<AVPacket, AVPacketDeleter> pkt;

    bool initializeEncoder(int width, int height, int fps, int64_t bitrate, 
                         const std::filesystem::path& outputFilePath) {
        logMessage("Initializing H264Encoder with parameters:");
        logMessage("Width: " + std::to_string(width));
        logMessage("Height: " + std::to_string(height));
        logMessage("FPS: " + std::to_string(fps));
        logMessage("Bitrate: " + std::to_string(bitrate));
        logMessage("Output file: " + outputFilePath.string());

        const AVCodec* codec = avcodec_find_encoder_by_name("libx264");
        if (!codec) {
            logMessage("Error: libx264 codec not found");
            return false;
        }

        c.reset(avcodec_alloc_context3(codec));
        if (!c) {
            logMessage("Error: Could not allocate codec context");
            return false;
        }

        c->bit_rate = std::min<int64_t>(bitrate, INT_MAX);
        c->width = width;
        c->height = height;
        c->time_base = {1, fps};
        c->framerate = {fps, 1};
        c->gop_size = 10;
        c->max_b_frames = 1;
        c->pix_fmt = AV_PIX_FMT_YUV420P;

        if (av_opt_set(c->priv_data, "preset", "medium", 0) < 0) {
            logMessage("Warning: Failed to set encoder preset");
        }

        if (avcodec_open2(c.get(), codec, nullptr) < 0) {
            logMessage("Error: Could not open codec");
            return false;
        }

        if (!setupFormatContext(outputFilePath)) {
            logMessage("Error: Failed to set up format context");
            return false;
        }

        if (!setupFrameAndPacket()) {
            logMessage("Error: Failed to set up frame and packet");
            return false;
        }

        return true;
    }

    bool setupFormatContext(const std::filesystem::path& outputFilePath) {
        AVFormatContext* fmt_ctx_ptr = nullptr;
        if (avformat_alloc_output_context2(&fmt_ctx_ptr, nullptr, nullptr, 
                                         outputFilePath.c_str()) < 0) {
            logMessage("Error: Could not allocate output context");
            return false;
        }
        fmt_ctx.reset(fmt_ctx_ptr);

        AVStream* stream = avformat_new_stream(fmt_ctx.get(), nullptr);
        if (!stream) {
            logMessage("Error: Could not create stream");
            return false;
        }

        if (avcodec_parameters_from_context(stream->codecpar, c.get()) < 0) {
            logMessage("Error: Could not copy codec parameters");
            return false;
        }

        if (avio_open(&fmt_ctx->pb, outputFilePath.c_str(), AVIO_FLAG_WRITE) < 0) {
            logMessage("Error: Could not open output file");
            return false;
        }

        if (avformat_write_header(fmt_ctx.get(), nullptr) < 0) {
            logMessage("Error: Could not write header");
            return false;
        }

        return true;
    }

    bool setupFrameAndPacket() {
        frame.reset(av_frame_alloc());
        if (!frame) {
            logMessage("Error: Could not allocate frame");
            return false;
        }

        frame->format = c->pix_fmt;
        frame->width = c->width;
        frame->height = c->height;
        
        if (av_frame_get_buffer(frame.get(), 32) < 0) {
            logMessage("Error: Could not allocate frame buffer");
            return false;
        }

        pkt.reset(av_packet_alloc());
        if (!pkt) {
            logMessage("Error: Could not allocate packet");
            return false;
        }

        return true;
    }

    bool writeFrameInternal(const std::vector<uint8_t>& frameData) {
        if (av_frame_make_writable(frame.get()) < 0) {
            logMessage("Error: Frame not writable");
            return false;
        }

        std::unique_ptr<AVFrame, AVFrameDeleter> rgbFrame(av_frame_alloc());
        if (!rgbFrame) {
            logMessage("Error: Could not allocate RGB frame");
            return false;
        }

        rgbFrame->format = AV_PIX_FMT_RGB24;
        rgbFrame->width = c->width;
        rgbFrame->height = c->height;
        
        if (av_frame_get_buffer(rgbFrame.get(), 0) < 0) {
            logMessage("Error: Could not allocate RGB buffer");
            return false;
        }

        const int stride = c->width * 3;
        for (int y = 0; y < c->height; y++) {
            memcpy(rgbFrame->data[0] + y * rgbFrame->linesize[0],
                   frameData.data() + y * stride,
                   stride);
        }

        SwsContext* sws_ctx = sws_getContext(
            c->width, c->height, AV_PIX_FMT_RGB24,
            c->width, c->height, AV_PIX_FMT_YUV420P,
            SWS_BILINEAR, nullptr, nullptr, nullptr
        );
        
        if (!sws_ctx) {
            logMessage("Error: Could not create SwsContext");
            return false;
        }
        
        sws_scale(sws_ctx, 
                 (const uint8_t* const*)rgbFrame->data, rgbFrame->linesize,
                 0, c->height,
                 frame->data, frame->linesize);
                 
        sws_freeContext(sws_ctx);

        frame->pts = m_frameCount++;
        return processEncodedFrame();
    }

    bool processEncodedFrame() {
        int ret = avcodec_send_frame(c.get(), frame.get());
        if (ret < 0) {
            logMessage("Error: Failed to send frame to encoder");
            return false;
        }

        while (ret >= 0) {
            ret = avcodec_receive_packet(c.get(), pkt.get());
            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                return true;
            }
            if (ret < 0) {
                logMessage("Error: Failed to receive packet from encoder");
                return false;
            }

            av_packet_rescale_ts(pkt.get(), c->time_base, fmt_ctx->streams[0]->time_base);
            pkt->stream_index = 0;
            
            if (av_interleaved_write_frame(fmt_ctx.get(), pkt.get()) < 0) {
                logMessage("Error: Failed to write packet to file");
                return false;
            }
            av_packet_unref(pkt.get());
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
        logMessage("Encoding process finished");
    }
};