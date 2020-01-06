#include "Codec.h"
#include <iostream>

using namespace std;


bool Codec::initFlag = false;

Codec::Codec()
{
	if (!Codec::initFlag)	//若未注册，则注册ffmpeg
		av_register_all();

	videoIndex = -1;
	audioIndex = -1;

	pFormatCtx = NULL;

	decode_flag = false;
}


Codec::~Codec(void)
{
	if(isDecode())
		endDecode();					//结束解码
}

int Codec::openMedia(char *route){
	//申请容器上下文
	pFormatCtx = avformat_alloc_context();

	//将媒体读入format中，正确读取返回0，否则为负数
	if(avformat_open_input(&pFormatCtx,route,NULL,NULL)!=0){
		cout<<"打开文件失败！"<<endl;	
		return -1;
	}

	//得到多媒体的流信息
	avformat_find_stream_info(pFormatCtx,NULL);

	//获取各个数据流的index
	for(int i = 0;i<pFormatCtx->nb_streams;i++){
		if(pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
			videoIndex = i;
		if(pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO)
			audioIndex = i;
	}
	if(audioIndex < 0){
		cout<<"没有音频"<<endl;
		return 0;
	}
	if(videoIndex < 0){
		cout<<"没有视频"<<endl;
		return 0;
	}

	//得到音视频的两个流
	pStream_video = pFormatCtx->streams[videoIndex];
	pStream_audio = pFormatCtx->streams[audioIndex];

	//得到音视频的两个解码上下文
	pCodecCtx_video = pStream_video->codec;
	pCodecCtx_audio = pStream_audio->codec;

	//找到音视频对应解码器
	if(!(pCodec_video = avcodec_find_decoder(pCodecCtx_video->codec_id)) || !(pCodec_audio = avcodec_find_decoder(pCodecCtx_audio->codec_id))){
		cout<<"未知的视频/音频编码格式"<<endl;
		return 0;
	}

	//打开音视频对应的解码器
	if(avcodec_open2(pCodecCtx_video,pCodec_video,NULL) < 0 || avcodec_open2(pCodecCtx_audio,pCodec_audio,NULL) < 0){
		cout<<"解码器打开失败"<<endl;
		return 0;
	}

	cout<<"成功打开文件！！"<<endl;
}

int Codec::closeMedia(){

	avformat_free_context(pFormatCtx);		//释放formatCtx
	pFormatCtx = NULL;
	return 0;
}

bool Codec::isDecode(){
	return decode_flag;
}

int Codec::decode(void *data){
	Codec *pkt = (Codec*)data;
	AVFormatContext *pFormatCtx = pkt->pFormatCtx;
	int videoIndex = pkt->videoIndex,audioIndex = pkt->audioIndex;
	AVCodecContext *pCodecCtx_video = pFormatCtx->streams[videoIndex]->codec;
	AVCodecContext *pCodecCtx_audeo = pFormatCtx->streams[audioIndex]->codec;
	FrameBuff *buff = pkt->buff;

	AVFrame *pFrame = av_frame_alloc();
	AVPacket *pPacket = NULL;	//申请packet
	int gotPicture;
	int ret;

	//从pFormatCtx中读一个packet
	while (pkt->isDecode())	
	{
		pPacket = av_packet_alloc();
		if(av_read_frame(pFormatCtx,pPacket)<0)
			break;

		if(pPacket->stream_index == videoIndex){		//视频packet
			cout<<"decode thread"<<endl;
			avcodec_decode_video2(pCodecCtx_video,pFrame,&gotPicture,pPacket);		//利用pCodecCtx,从packet解码到pFrame中
			
			if(gotPicture){		//表示解码了一帧到pFrame中
				while(buff->isFull() && pkt->isDecode());
				buff->push(pFrame->data[0],pFrame->data[1],pFrame->data[2],pFrame->pkt_pts);
			}
		}else if(pPacket->stream_index == audioIndex){	//音频packet

		}
		av_packet_free(&pPacket);
	}

	cout<<"decode thread enddddddd!";

	av_frame_free(&pFrame);

// 	int width = pCodecCtx_video->width;
// 	int height = pCodecCtx_video->height;
// 	int Ysize = width * height;
// 	int UVsize = width * height / 4;
// 	uint8_t *y = new uint8_t[Ysize];
// 	uint8_t *u = new uint8_t[UVsize];
// 	uint8_t *v = new uint8_t[UVsize];
// 	while(threadRunFlag){
// 		for(int i = 0;i<Ysize;i++)
// 			y[i] = rand()%256;
// 		for(int i = 0;i<UVsize;i++){
// 			u[i] = rand()%256;
// 			v[i] = rand()%256;
// 		}
// 
// 		while(buff->isFull() && threadRunFlag);
// 		buff->push(y,u,v);
// 		cout<<"decode thread!"<<endl;
// 	}
// 	cout<<"decode thread endddddd!"<<endl;



	return 0;
}

void Codec::startDecode(FrameBuff *buff){
	this->buff = buff;
	decode_flag = true;
	thread = SDL_CreateThread(decode,"decode",this);	//开始解码线程
}

void Codec::endDecode(){
	decode_flag = false;			//通知解码线程结束
	SDL_WaitThread(thread,NULL);	//等待解码线程结束
}

AVCodecContext *Codec::getCodecCtx_video(){
	return pCodecCtx_video;
}

AVFormatContext *Codec::getFormatCtx(){
	return pFormatCtx;
}

void Codec::forward(int sec){
	av_seek_frame(pFormatCtx,videoIndex,buff->top()->getPts()+ pStream_video->avg_frame_rate.num * pCodecCtx_video->time_base.den / pStream_video->avg_frame_rate.den / pCodecCtx_video->time_base.num * sec,AVSEEK_FLAG_FRAME);
}

void Codec::backward(int sec){
	av_seek_frame(pFormatCtx,videoIndex,buff->top()->getPts()- pStream_video->avg_frame_rate.num * pCodecCtx_video->time_base.den / pStream_video->avg_frame_rate.den / pCodecCtx_video->time_base.num * sec,AVSEEK_FLAG_FRAME);
}