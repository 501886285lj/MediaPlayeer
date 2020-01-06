#pragma once
#include "Frame.h"
extern "C"{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_thread.h>
#undef main
};

class Codec
{
	//ffmpeg
	static bool initFlag;
	AVFormatContext *pFormatCtx;						//����������
	AVStream *pStream_video,*pStream_audio;				//������
	AVCodecContext *pCodecCtx_video, *pCodecCtx_audio;	//������������
	AVCodec *pCodec_video,*pCodec_audio;				//������
	int videoIndex,audioIndex;
	FrameBuff *buff;
	SwsContext *pConvertCtx;

	bool decode_flag;
	SDL_Thread *thread;

public:
	Codec();
	
	int openMedia(char *route);
	int closeMedia();

	void startDecode(FrameBuff *buff);
	void endDecode();

	bool isDecode();

	AVCodecContext *getCodecCtx_video();
	AVFormatContext *getFormatCtx();

	static int decode(void *data);

	void forward(int sec);
	void backward(int sec);

	~Codec(void);
};