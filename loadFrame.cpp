// https://www.youtube.com/watch?v=MEMzo59CPr8
// 2:08

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}
#include <cstdint>
#include <iostream>

bool loadFrame(const char* fileName, int* widthOut, int* heightOut, unsigned char** dataOut)
{
	// open the file using libavformat
	AVFormatContext* avFormatCtx = avformat_alloc_context();
	if (!avFormatCtx)
	{
		std::cout << "coundln't create AVFormatContext\n";
		return false;
	}

	if (avformat_open_input(&avFormatCtx, fileName, nullptr, nullptr) < 0)
	{
		// should the context be freed here on error?
		std::cout << "couldn't open video file" << '\n';
		return false;
	}

	// find first valid video stream
	int videoStreamIndex = -1;
	AVCodecParameters* avCodecParams = nullptr;
	AVCodec* avCodec = nullptr;
	for (int i = 0; i < avFormatCtx->nb_streams; ++i)
	{
		auto stream = avFormatCtx->streams[i];
		avCodecParams = avFormatCtx->streams[i]->codecpar;
		avCodec = avcodec_find_decoder(avCodecParams->codec_id);
		if (!avCodec) { continue; }
		if (avCodecParams->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			videoStreamIndex = i;
			break;
		}
	}

	if (videoStreamIndex == -1)
	{
		std::cout << "couldn't find valid stream index\n";
		return false;
	}
	
	// setup a codec context for the decoder
	AVCodecContext* avCodecCtx = avcodec_alloc_context3(avCodec);
	if (!avCodecCtx)
	{
		std::cout << "failed to alloc codec ctx!\n";
		return false;
	}
	if (avcodec_parameters_to_context(avCodecCtx, avCodecParams) < 0)
	{
		std::cout << "failed to create ctx!\n";
		return false;
	}
	if (avcodec_open2(avCodecCtx, avCodec, nullptr) < 0)
	{
		std::cout << "failed to open codec!\n";
		return false;
	}

	AVFrame* avFrame = av_frame_alloc();
	if (!avFrame) { std::cout << "frame alloc failed\n"; return false; }
	AVPacket* avPacket = av_packet_alloc();
	if (!avPacket) { std::cout << "packet alloc failed\n"; return false; }
	int response = 0;
	while (av_read_frame(avFormatCtx, avPacket) >= 0)
	{
		if (avPacket->stream_index != videoStreamIndex) { continue; }
		// send packet
		if (response = (avcodec_send_packet(avCodecCtx, avPacket)) < 0)
		{
			std::cout << "failed to decode packet\n"; 
			return false;
		}
		// receive frame
		response = avcodec_receive_frame(avCodecCtx, avFrame);
		if (response == AVERROR(EAGAIN) || response == AVERROR_EOF) {
			continue;
		}
		else if (response < 0)
		{
			std::cout << "failed to decode packet\n";
			return false;
		}

		av_packet_unref(avPacket);
		break;

	}

	// set up conversion from YUV to RGBA

	SwsContext* swsScalerCtx = sws_getContext(avFrame->width, avFrame->height, avCodecCtx->pix_fmt,
												avFrame->width, avFrame->height, AV_PIX_FMT_RGBA,
												SWS_BILINEAR, nullptr, nullptr, nullptr);
	if (!swsScalerCtx)
	{
		std::cout << "couldn't init scaler!\n";
		return false;
	}
	uint8_t* data = new uint8_t[avFrame->width * avFrame->height * 4];
	uint8_t* dest[4] = { data, nullptr, nullptr, nullptr };
	int dest_linesize[4] = { avFrame->width * 4, 0, 0, 0 };
	sws_scale(swsScalerCtx, avFrame->data, avFrame->linesize, 0, avFrame->height, dest, dest_linesize);
	sws_freeContext(swsScalerCtx);

	*widthOut = avFrame->width;
	*heightOut = avFrame->height;
	*dataOut = data;

	//// avFrame is currently in YUV format, this puts out grayscale
	//unsigned char* data = new unsigned char[avFrame->width * avFrame->height * 3];
	//for (int x = 0; x < avFrame->width; ++x)
	//{
	//	for (int y = 0; y < avFrame->height; ++y)
	//	{
	//		data[y * avFrame->width * 3 + x * 3    ] = avFrame->data[0][y * avFrame->linesize[0] + x];
	//		data[y * avFrame->width * 3 + x * 3 + 1] = avFrame->data[0][y * avFrame->linesize[0] + x];
	//		data[y * avFrame->width * 3 + x * 3 + 2] = avFrame->data[0][y * avFrame->linesize[0] + x];	
	//	}
	//}

	//

	//std::cout << "AvFrame dimensions: " << avFrame->width << '*' << avFrame->height << '\n';

	//*widthOut = avFrame->width;
	//*heightOut = avFrame->height;
	//*dataOut = data;

	// cleanup
	//delete[] data;
	avformat_close_input(&avFormatCtx);
	avformat_free_context(avFormatCtx);
	avcodec_free_context(&avCodecCtx);
	av_frame_free(&avFrame);
	av_packet_free(&avPacket);
	


	return true;
}