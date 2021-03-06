#include "VideoReader.h"


VideoReader::VideoReader(const char* fileName) 
{
	// open file and bind to avFormatCtx
	this->avFormatCtx = avformat_alloc_context();
	if (avformat_open_input(&avFormatCtx, fileName, nullptr, nullptr) < 0){ throw std::runtime_error("couldn't open video file"); }

	// find first valid video stream
	for (uint8_t i = 0; i < avFormatCtx->nb_streams; ++i)
	{
		auto stream = avFormatCtx->streams[i];
		avCodecParams = avFormatCtx->streams[i]->codecpar;
		
		avCodec = avcodec_find_decoder(avCodecParams->codec_id);
		if (!avCodec) { continue; }
		if (avCodecParams->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			videoStreamIndex = i;
			avTimeBase = avFormatCtx->streams[i]->time_base;
			break;
		}
	}

	if (videoStreamIndex == -1){ throw std::runtime_error("couldn't find video stream"); }

	// setup a codec context and get code params for the decoder
	this->avCodecCtx = avcodec_alloc_context3(avCodec);
	if (!avCodecCtx) { throw std::runtime_error("couldn't alloc codec ctx"); }
	if (avcodec_parameters_to_context(avCodecCtx, avCodecParams) < 0) { throw std::runtime_error("couldn't create codec ctx"); }
	if (avcodec_open2(avCodecCtx, avCodec, nullptr) < 0) { throw std::runtime_error("couldn't open codec"); }
	
	// allocate memeory for packet and frame
	this->avFrame = av_frame_alloc();
	if (!avFrame) { throw std::runtime_error("couldn't alloc av frame"); }
	this->avPacket = av_packet_alloc();
	if (!avPacket) { throw std::runtime_error("couldn't alloc packet"); }

	// allocate frameBuffer
	this->frameBuffer = new uint8_t[avCodecCtx->width * avCodecCtx->height * 4];
	
}

VideoReader::~VideoReader()
{
	delete frameBuffer;
	avformat_close_input(&avFormatCtx);
	avformat_free_context(avFormatCtx);
	avcodec_free_context(&avCodecCtx);
	av_frame_free(&avFrame);
	av_packet_free(&avPacket);
	sws_freeContext(swsScalerCtx);
}


uint8_t* VideoReader::readFrame()
{
	int response = 0;
	while (av_read_frame(avFormatCtx, avPacket) >= 0)
	{
		if (avPacket->stream_index != videoStreamIndex) 
		{
			av_packet_unref(avPacket);
			continue; 
		}
		// send packet
		if (response = (avcodec_send_packet(avCodecCtx, avPacket)) < 0) { throw std::runtime_error("couldn't send packet"); }
		// receive frame
		response = avcodec_receive_frame(avCodecCtx, avFrame);
		if (response == AVERROR(EAGAIN) || response == AVERROR_EOF) 
		{ 
			av_packet_unref(avPacket);
			continue; 
		}
		else if (response < 0) { throw std::runtime_error("couldn't decode packet"); }
		av_packet_unref(avPacket);
		break;
	}

	// setup scaler for colour conversion
	// set up conversion from YUV to RGBA
	// initialize swsSxcalerCtx the first time readFrame is called
	if (!swsScalerCtx)
	{
		this->swsScalerCtx = sws_getContext(avFrame->width, avFrame->height, avCodecCtx->pix_fmt,
			avFrame->width, avFrame->height, AV_PIX_FMT_RGBA,
			SWS_BILINEAR, nullptr, nullptr, nullptr);
	}
	if (!swsScalerCtx) { throw std::runtime_error("couldn't create scaler ctx"); }

	uint8_t* dest[4] = { frameBuffer, nullptr, nullptr, nullptr };
	int dest_linesize[4] = { avFrame->width * 4, 0, 0, 0 };
	sws_scale(swsScalerCtx, avFrame->data, avFrame->linesize, 0, avFrame->height, dest, dest_linesize);
	
	return frameBuffer;

}

int VideoReader::getHeight() const
{
	if (!avCodecParams) { throw std::runtime_error("codec params not filled yet"); }
	return avCodecParams->height;
}

int VideoReader::getWidth() const
{
	if(!avCodecParams) { throw std::runtime_error("codec params not filled yet"); }
	return avCodecParams->width;
}

// has to be called after first call to readFrame
double VideoReader::returnPtsInSecs()
{
	return avFrame->pts * (double)avTimeBase.num / (double)avTimeBase.den;
}
