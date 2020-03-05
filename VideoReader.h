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
	AVRational avTimeBase;

	int videoStreamIndex = -1;
	uint8_t* frameBuffer = nullptr;
	uint64_t pts; // presentation time 
	

public:
	VideoReader(const char* fileName);
	~VideoReader();

	uint8_t* readFrame();
	int getHeight() const;
	int getWidth() const;
	double returnPtsInSecs();

};



