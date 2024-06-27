extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <inttypes.h>
}



bool load_frame(const char* filename, int* width_out, int* height_out, unsigned char** data_out){
    // open video file using libavformat.
    // first create a AVFormatContext to put data into it:
    AVFormatContext* av_format_ctx = avformat_alloc_context();
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
    AVCodecParameters* av_codec_params;
    AVCodec* av_codec;
    int video_stream_index = -1;
    for(int i=0 ; i<av_format_ctx->nb_streams ; ++i){
        auto stream = av_format_ctx->streams[i];
        av_codec_params = av_format_ctx->streams[i]->codecpar;
        av_codec = avcodec_find_decoder(av_codec_params->codec_id);
        if(!av_codec){ continue; }
        if(av_codec_params->codec_type == AVMEDIA_TYPE_VIDEO){
            video_stream_index = i;
            break;
        }
    }
    if(video_stream_index == -1){
        printf("error: couldnt find video stream\n");
        return false;
    }

    // find auto stream:
    int audio_stream_index = -1;
    // ...

    // set up codec context for the decoder (allocate memory for decoder to store interal data for decoding)
    AVCodecContext* av_codec_ctx = avcodec_alloc_context3(av_codec);
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
    if(avcodec_open2(av_codec_ctx, av_codec, NULL) < 0)
    {
        printf("error: couldnt open codec\n");
        return false;
    }

    // now extract data 
    // allocate memory for frames and packets
    AVFrame* av_frame = av_frame_alloc();
    if(!av_frame){
        printf("error: couldnt allocate AVFrame\n");
        return false;
    }
    AVPacket* av_packet = av_packet_alloc();
    if(!av_packet){
        printf("error: couldnt allocate AVPacket\n");
        return false;
    }
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

    uint8_t* data = new uint8_t[av_frame->width * av_frame->height * 4];
    SwsContext* sws_scaler_ctx = sws_getContext(av_frame->width, av_frame->height, av_codec_ctx->pix_fmt, // input 
                                                av_frame->width, av_frame->height, AV_PIX_FMT_RGB0,       // output
                                                SWS_BILINEAR,NULL,NULL,NULL);                             // options
    if(!sws_scaler_ctx){
        printf("error: couldnt initialize swscaler\n");
        return false;
    }

    uint8_t * dest[4] = {data,NULL,NULL,NULL};
    int dest_linesize[4] = {av_frame->width * 4, 0, 0 ,0} ;
    sws_scale(sws_scaler_ctx, av_frame->data, av_frame->linesize, 0, av_frame->height, dest, dest_linesize);
    sws_freeContext(sws_scaler_ctx);

    *width_out = av_frame->width;
    *height_out = av_frame->height;
    *data_out = data;


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

    *data_out = data;
*/



    avformat_close_input(&av_format_ctx);
    avformat_free_context(av_format_ctx);
    av_frame_free(&av_frame);
    av_packet_free(&av_packet);
    avcodec_free_context(&av_codec_ctx);

    return true;
}








/*
  for testin
  bool load_frame(const char* filename, int* width, int* height, unsigned char** data){
    *width = 100;
    *height = 100;
    *data = new unsigned char[100 * 100 * 3];
    auto ptr = *data;
    for(int i{} ; i<100 ; ++i){
        for(int y{} ; y<100 ; ++y){
            *ptr++ = 0xff;
            *ptr++ = 0x00;
            *ptr++ = 0x00;
        }
    }
    return true;
}
*/