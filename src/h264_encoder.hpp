#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <cstring>

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
    H264Encoder(int width, int height, int fps, int64_t bitrate, const std::filesystem::path& outputFilePath) {
        int ret;
        std::string filenameStr = outputFilePath.string();
        const char *filename = filenameStr.c_str();

        logMessage("== H264 Encoder ==");
        logMessage("Initializing H264Encoder with parameters:");
        logMessage("Width: " + std::to_string(width));
        logMessage("Height: " + std::to_string(height));
        logMessage("FPS: " + std::to_string(fps));
        logMessage("Bitrate: " + std::to_string(bitrate));
        logMessage("Output file: " + filenameStr);

        const AVCodec *codec = avcodec_find_encoder_by_name("libx264");
        if (!codec) {
            logMessage("Error: Could not find libx264 codec.");
            return;
        }

        c = avcodec_alloc_context3(codec);
        if (!c) {
            logMessage("Error: Could not allocate codec context.");
            return;
        }

        ret = avformat_alloc_output_context2(&fmt_ctx, NULL, NULL, filename);
        if (!fmt_ctx) {
            logMessage("Error: Could not allocate output context.");
            return;
        }

        AVStream *stream = avformat_new_stream(fmt_ctx, codec);
        if (!stream) {
            logMessage("Error: Could not create new stream.");
            return;
        }

        c->bit_rate = bitrate;
        c->width = width;
        c->height = height;
        c->time_base = (AVRational){1, fps};
        c->framerate = (AVRational){fps, 1};
        c->gop_size = 10;
        c->max_b_frames = 1;
        c->pix_fmt = AV_PIX_FMT_YUV420P;

        av_opt_set(c->priv_data, "preset", "medium", 0);

        stream->time_base = c->time_base;

        ret = avcodec_open2(c, codec, NULL);
        if (ret < 0) {
            logMessage("Error: Could not open codec.");
            return;
        }

        ret = avcodec_parameters_from_context(stream->codecpar, c);
        if (ret < 0) {
            logMessage("Error: Could not copy codec parameters to stream.");
            return;
        }

        ret = avio_open(&fmt_ctx->pb, filename, AVIO_FLAG_WRITE);
        if (ret < 0) {
            logMessage("Error: Could not open output file.");
            return;
        }

        ret = avformat_write_header(fmt_ctx, NULL);
        if (ret < 0) {
            logMessage("Error: Could not write header to output file.");
            return;
        }

        frame = av_frame_alloc();
        if (!frame) {
            logMessage("Error: Could not allocate video frame.");
            return;
        }

        frame->format = c->pix_fmt;
        frame->width = c->width;
        frame->height = c->height;

        ret = av_frame_get_buffer(frame, 0);
        if (ret < 0) {
            logMessage("Error: Could not allocate frame buffer.");
            return;
        }

        pkt = av_packet_alloc();
        if (!pkt) {
            logMessage("Error: Could not allocate packet.");
            return;
        }

        logMessage("H264Encoder initialized successfully.");
    }

    int finish() {
        logMessage("Finishing encoding process...");

        int result = encode_video_frame(c, NULL, pkt, fmt_ctx);
        if (result < 0) {
            logMessage("Error: Failed to encode final frame.");
            return 0;
        }

        av_write_trailer(fmt_ctx);

        if (fmt_ctx && fmt_ctx->pb) {
            avio_closep(&fmt_ctx->pb);
        }
        avformat_free_context(fmt_ctx);
        avcodec_free_context(&c);
        av_frame_free(&frame);
        av_packet_free(&pkt);

        logMessage("Encoding process finished successfully.");
        logMessage("== The End ==");

        return result;
    }

    bool writeFrame(const std::vector<uint8_t>& frameData) {
        int ret;

        ret = av_frame_make_writable(frame);
        if (ret < 0) {
            logMessage("Error: Could not make frame writable.");
            return false;
        }

        AVFrame* tempFrame = av_frame_alloc();
        if (!tempFrame) {
            logMessage("Error: Could not allocate temporary frame.");
            return false;
        }

        tempFrame->format = AV_PIX_FMT_RGB24;
        tempFrame->width = c->width;
        tempFrame->height = c->height;

        ret = av_frame_get_buffer(tempFrame, 0);
        if (ret < 0) {
            logMessage("Error: Could not allocate temporary frame buffer.");
            av_frame_free(&tempFrame);
            return false;
        }

        memcpy(tempFrame->data[0], frameData.data(), frameData.size());

        SwsContext* swsCtx = sws_getContext(c->width, c->height, AV_PIX_FMT_RGB24,
                                            c->width, c->height, AV_PIX_FMT_YUV420P,
                                            0, nullptr, nullptr, nullptr);
        if (!swsCtx) {
            logMessage("Error: Could not create SwsContext.");
            av_frame_free(&tempFrame);
            return false;
        }

        ret = sws_scale(swsCtx, tempFrame->data, tempFrame->linesize, 0, c->height,
                        frame->data, frame->linesize);
        if (ret < 0) {
            logMessage("Error: Could not scale frame.");
            sws_freeContext(swsCtx);
            av_frame_free(&tempFrame);
            return false;
        }

        sws_freeContext(swsCtx);
        av_frame_free(&tempFrame);

        frame->pts = m_frameCount++;
        ret = encode_video_frame(c, frame, pkt, fmt_ctx);
        if (ret < 0) {
            logMessage("Error: Could not encode video frame.");
            return false;
        }
        
        return true;
    }

private:
    AVCodecContext *c = nullptr;
    AVFormatContext *fmt_ctx = nullptr;
    AVFrame *frame = nullptr;
    AVPacket *pkt = nullptr;
    
    int m_frameCount = 0;

    int encode_video_frame(AVCodecContext *enc_ctx, AVFrame *frame, AVPacket *pkt, AVFormatContext *fmt_ctx) {
        int ret;

        ret = avcodec_send_frame(enc_ctx, frame);
        if (ret < 0) {
            logMessage("Error: Could not send frame to encoder.");
            return ret;
        }

        while (ret >= 0) {
            ret = avcodec_receive_packet(enc_ctx, pkt);
            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
                return 0;
            else if (ret < 0) {
                logMessage("Error: Could not receive packet from encoder.");
                return ret;
            }

            av_packet_rescale_ts(pkt, enc_ctx->time_base, fmt_ctx->streams[0]->time_base);
            pkt->stream_index = 0;

            ret = av_interleaved_write_frame(fmt_ctx, pkt);
            if (ret < 0) {
                logMessage("Error: Could not write packet to file.");
                return ret;
            }
            av_packet_unref(pkt);
        }

        return 0;
    }  
};