#pragma once

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}
#include <cstdint>
#include <iostream>

class VideoReader
{
private:

	AVFormatContext* avFormatCtx = nullptr;
	AVCodecContext* avCodecCtx = nullptr;
	SwsContext* swsScalerCtx = nullptr;
	AVFrame* avFrame = nullptr;
	AVPacket* avPacket = nullptr;
	AVCodecParameters* avCodecParams = nullptr;
	AVCodec* avCodec = nullptr;
	int videoStreamIndex = -1;
public:
	VideoReader(const char* fileName);
	~VideoReader();

	uint8_t* readFrame();
	
	const int getHeight();
	const int getWidth();
};



