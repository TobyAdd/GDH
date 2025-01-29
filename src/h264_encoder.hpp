extern "C" {
    #include <libavcodec/avcodec.h>
    #include <libavutil/opt.h>
    #include <libavutil/imgutils.h>
    #include <libavformat/avformat.h>
    #include <libswscale/swscale.h>
}

#include <filesystem>

class H264Encoder {
public:
    H264Encoder(int width, int height, int fps, int64_t bitrate, const std::filesystem::path& outputFilePath) {
        int ret;
        std::string filenameStr = outputFilePath.string();
        const char *filename = filenameStr.c_str();

        const AVCodec *codec = avcodec_find_encoder_by_name("libx264");
        if (!codec) {
            return;
        }

        c = avcodec_alloc_context3(codec);
        if (!c) {
            return;
        }

        ret = avformat_alloc_output_context2(&fmt_ctx, NULL, NULL, filename);
        if (!fmt_ctx) {
            return;
        }

        AVStream *stream = avformat_new_stream(fmt_ctx, codec);
        if (!stream) {
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
            return;
        }

        ret = avcodec_parameters_from_context(stream->codecpar, c);
        if (ret < 0) {
            return;
        }

        ret = avio_open(&fmt_ctx->pb, filename, AVIO_FLAG_WRITE);
        if (ret < 0) {
            return;
        }

        ret = avformat_write_header(fmt_ctx, NULL);
        if (ret < 0) {
            return;
        }

        frame = av_frame_alloc();
        if (!frame) {
            return;
        }

        frame->format = c->pix_fmt;
        frame->width = c->width;
        frame->height = c->height;

        ret = av_frame_get_buffer(frame, 0);
        if (ret < 0) {
            return;
        }

        pkt = av_packet_alloc();
        if (!pkt) {
            return;
        }
    }

    int finish() {
        int result = encode_video_frame(c, NULL, pkt, fmt_ctx);
        if (result < 0) {
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

        return result;
    }

    ~H264Encoder() {
        finish();
    }

    bool writeFrame(const std::vector<uint8_t>& frameData) {
        int ret;

        ret = av_frame_make_writable(frame);
        if (ret < 0) {
            return false;
        }

        AVFrame* tempFrame = av_frame_alloc();
        if (!tempFrame) {
            return false;
        }

        tempFrame->format = AV_PIX_FMT_RGB24;
        tempFrame->width = c->width;
        tempFrame->height = c->height;

        ret = av_frame_get_buffer(tempFrame, 0);
        if (ret < 0) {
            av_frame_free(&tempFrame);
            return false;
        }

        memcpy(tempFrame->data[0], frameData.data(), frameData.size());

        SwsContext* swsCtx = sws_getContext(c->width, c->height, AV_PIX_FMT_RGB24,
                                            c->width, c->height, AV_PIX_FMT_YUV420P,
                                            0, nullptr, nullptr, nullptr);
        if (!swsCtx) {
            av_frame_free(&tempFrame);
            return false;
        }

        ret = sws_scale(swsCtx, tempFrame->data, tempFrame->linesize, 0, c->height,
                        frame->data, frame->linesize);
        if (ret < 0) {
            sws_freeContext(swsCtx);
            av_frame_free(&tempFrame);
            return false;
        }

        sws_freeContext(swsCtx);
        av_frame_free(&tempFrame);

        frame->pts = m_frameCount++;
        ret = encode_video_frame(c, frame, pkt, fmt_ctx);
        if (ret < 0) {
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

    int encode_video_frame(AVCodecContext *enc_ctx, AVFrame *frame, AVPacket *pkt, AVFormatContext *fmt_ctx)
    {
        int ret;

        ret = avcodec_send_frame(enc_ctx, frame);
        if (ret < 0) {
            return ret;
        }

        while (ret >= 0) {
            ret = avcodec_receive_packet(enc_ctx, pkt);
            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
                return 0;
            else if (ret < 0) {
                return ret;
            }

            av_packet_rescale_ts(pkt, enc_ctx->time_base, fmt_ctx->streams[0]->time_base);
            pkt->stream_index = 0;

            ret = av_interleaved_write_frame(fmt_ctx, pkt);
            if (ret < 0) {
                return ret;
            }
            av_packet_unref(pkt);
        }

        return 0;
    }  
};