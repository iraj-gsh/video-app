#include <stdio.h>
#include "video_reader.hpp"

bool video_reader_open_file(VideoReaderState* state, const char* filename){
    auto& width = state->width;
    auto& height = state->height;
    auto& av_format_ctx = state->av_format_ctx;
    auto& av_codec_ctx = state->av_codec_ctx;
    auto& av_frame = state->av_frame;
    auto& av_packet = state->av_packet;
    auto& sws_scaler_ctx = state->sws_scaler_ctx;
    auto& video_stream_index = state->video_stream_index;

    // open video file using libavformat.
    // first create a AVFormatContext to put data into it:
    av_format_ctx = avformat_alloc_context();
    if(!av_format_ctx){ // check for errors
        printf("error: couldnt create AVFormatContext\n");
        return false;
    }
    // open a video into aforementioned context:
    if(avformat_open_input(&av_format_ctx, filename, NULL,NULL) != 0){
        printf("error: couldnt open video file\n");
        return false;
    }
    // find the first valid video stream inside AVFormatContext
    // access streams of data in the AVFromatContext 
    video_stream_index = -1;
    AVCodecParameters* av_codec_params;
    AVCodec* av_codec;
    for(int i=0 ; i<av_format_ctx->nb_streams ; ++i){
        av_codec_params = av_format_ctx->streams[i]->codecpar;
        av_codec = avcodec_find_decoder(av_codec_params->codec_id);
        if(!av_codec){ continue; }
        if(av_codec_params->codec_type == AVMEDIA_TYPE_VIDEO){
            video_stream_index = i;
            width = av_codec_params->width;
            height = av_codec_params->height;
            break;
        }
    }
    if(video_stream_index == -1){
        printf("error: couldnt find video stream\n");
        return false;
    }
    // set up codec context for the decoder (allocate memory for decoder to store interal data for decoding)
    av_codec_ctx = avcodec_alloc_context3(av_codec);
    if(!av_codec_ctx){
        printf("error: couldnt create AVCodecContext\n");
        return false;
    }
    // put information taken from video file by libavformat into AVCodecContext as its initial state:
    if(avcodec_parameters_to_context(av_codec_ctx, av_codec_params) < 0){
        printf("error: couldnt initialize AVCodecContex\n");
        return false;
    }

    // now open the av_codec_ctx to read
    if(avcodec_open2(av_codec_ctx, av_codec, NULL) < 0){
        printf("error: couldnt open codec\n");
        return false;
    }

    // now extract data 
    // allocate memory for frames and packets
    av_frame = av_frame_alloc();
    if(!av_frame){
        printf("error: couldnt allocate AVFrame\n");
        return false;
    }
    av_packet = av_packet_alloc();
    if(!av_packet){
        printf("error: couldnt allocate AVPacket\n");
        return false;
    }
    return true;
}


bool video_reader_read_frame(VideoReaderState* state, uint8_t* frame_buffer){
    // unpack VideoReaderState data
    auto& width = state->width;
    auto& height = state->height;
    auto& av_format_ctx = state->av_format_ctx;
    auto& av_codec_ctx = state->av_codec_ctx;
    auto& av_frame = state->av_frame;
    auto& av_packet = state->av_packet;
    auto& sws_scaler_ctx = state->sws_scaler_ctx;
    auto& video_stream_index = state->video_stream_index;

    // decode one frame
    int response;
    while(av_read_frame(av_format_ctx, av_packet) >= 0 ){
        // iterate over packets until the packet belongs to video stream
        if(av_packet->stream_index != video_stream_index){
            continue;
        }
        // now we have a video packet:
        response = avcodec_send_packet(av_codec_ctx, av_packet);
        if(response < 0){
            printf("error: failed to decodec packet\n");
            return false;
        }
        
        response = avcodec_receive_frame(av_codec_ctx, av_frame);
        if(response == AVERROR(EAGAIN) || response == AVERROR_EOF){
            continue; // packet is either decoded before or is empty
        } else if (response < 0){
            printf("error: failed to decode packet\n");
            return false;
        }

        av_packet_unref(av_packet); // you gave me a packet now i dont wanna use it anymore
        break;
    }
    if(!sws_scaler_ctx){
        // set up sws scaler context:
        sws_scaler_ctx = sws_getContext(width, height, av_codec_ctx->pix_fmt, // input 
                                        width, height, AV_PIX_FMT_RGB0,       // output
                                        SWS_BILINEAR,NULL,NULL,NULL);                             // options
    }

    printf(
        "Frame %c (%d) pts %d dts %d key_frame %d [coded_picture_number %d, display_picture_number %d]\n",
        av_get_picture_type_char(av_frame->pict_type),
        av_codec_ctx->frame_number,
        av_frame->pts,
        av_frame->pkt_dts,
        av_frame->key_frame,
        av_frame->coded_picture_number,
        av_frame->display_picture_number
    );

    if(!sws_scaler_ctx){
    printf("error: couldnt initialize swscaler\n");
    return false;
    }
    

    uint8_t* data = new uint8_t[width * height * 4];
    uint8_t * dest[4] = {frame_buffer,NULL,NULL,NULL};
    int dest_linesize[4] = {width * 4, 0, 0 ,0} ;
    sws_scale(sws_scaler_ctx, av_frame->data, av_frame->linesize, 0, av_frame->height, dest, dest_linesize);



/*
    unsigned char* data = new unsigned char[av_frame->width * av_frame->height * 3];
    *height_out = av_frame->height;
    *width_out = av_frame->width;
    for(int x{} ; x<av_frame->width ; ++x){
        for(int y{} ; y<av_frame->height ; ++y){
            data[y*av_frame->width*3 + x*3   ] = av_frame->data[0][y * av_frame->linesize[0] + x];
            data[y*av_frame->width*3 + x*3+1 ] = av_frame->data[0][y * av_frame->linesize[0] + x];
            data[y*av_frame->width*3 + x*3+2 ] = av_frame->data[0][y * av_frame->linesize[0] + x];
        }
    }

    *width_out = av_frame->width;
    *height_out = av_frame->height;
    *data_out = data;

*/


    

    return true;

}

void video_reader_close(VideoReaderState* state){
    sws_freeContext(state->sws_scaler_ctx);
    avformat_close_input(&state->av_format_ctx);
    avformat_free_context(state->av_format_ctx);
    av_frame_free(&state->av_frame);
    av_packet_free(&state->av_packet);
    avcodec_free_context(&state->av_codec_ctx);
}