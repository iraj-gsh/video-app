#ifndef VIDEO_READER_HPP
#define VIDEO_READER_HPP


extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <inttypes.h>
}

struct VideoReaderState{

    // public data members:
    int width,height;

    // private data members
    AVFormatContext* av_format_ctx;
    AVCodecContext* av_codec_ctx;
    AVFrame* av_frame;
    AVPacket* av_packet;
    SwsContext* sws_scaler_ctx;
    int video_stream_index;
};

// we need 3 functions: open_file(), read_frame(), free()

bool video_reader_open_file(VideoReaderState* state, const char* filename);
bool video_reader_read_frame(VideoReaderState* state, uint8_t* frame_buffer);
void video_reader_close(VideoReaderState* state);



#endif //VIDEO_READER_HPP