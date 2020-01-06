#include "Codec.h"
#include <iostream>

using namespace std;


bool Codec::initFlag = false;

Codec::Codec()
{
	if (!Codec::initFlag)	//��δע�ᣬ��ע��ffmpeg
		av_register_all();

	videoIndex = -1;
	audioIndex = -1;

	pFormatCtx = NULL;

	decode_flag = false;
}


Codec::~Codec(void)
{
	if(isDecode())
		endDecode();					//��������
}

int Codec::openMedia(char *route){
	//��������������
	pFormatCtx = avformat_alloc_context();

	//��ý�����format�У���ȷ��ȡ����0������Ϊ����
	if(avformat_open_input(&pFormatCtx,route,NULL,NULL)!=0){
		cout<<"���ļ�ʧ�ܣ�"<<endl;	
		return -1;
	}

	//�õ���ý�������Ϣ
	avformat_find_stream_info(pFormatCtx,NULL);

	//��ȡ������������index
	for(int i = 0;i<pFormatCtx->nb_streams;i++){
		if(pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
			videoIndex = i;
		if(pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO)
			audioIndex = i;
	}
	if(audioIndex < 0){
		cout<<"û����Ƶ"<<endl;
		return 0;
	}
	if(videoIndex < 0){
		cout<<"û����Ƶ"<<endl;
		return 0;
	}

	//�õ�����Ƶ��������
	pStream_video = pFormatCtx->streams[videoIndex];
	pStream_audio = pFormatCtx->streams[audioIndex];

	//�õ�����Ƶ����������������
	pCodecCtx_video = pStream_video->codec;
	pCodecCtx_audio = pStream_audio->codec;

	//�ҵ�����Ƶ��Ӧ������
	if(!(pCodec_video = avcodec_find_decoder(pCodecCtx_video->codec_id)) || !(pCodec_audio = avcodec_find_decoder(pCodecCtx_audio->codec_id))){
		cout<<"δ֪����Ƶ/��Ƶ�����ʽ"<<endl;
		return 0;
	}

	//������Ƶ��Ӧ�Ľ�����
	if(avcodec_open2(pCodecCtx_video,pCodec_video,NULL) < 0 || avcodec_open2(pCodecCtx_audio,pCodec_audio,NULL) < 0){
		cout<<"��������ʧ��"<<endl;
		return 0;
	}

	cout<<"�ɹ����ļ�����"<<endl;
}

int Codec::closeMedia(){

	avformat_free_context(pFormatCtx);		//�ͷ�formatCtx
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
	AVPacket *pPacket = NULL;	//����packet
	int gotPicture;
	int ret;

	//��pFormatCtx�ж�һ��packet
	while (pkt->isDecode())	
	{
		pPacket = av_packet_alloc();
		if(av_read_frame(pFormatCtx,pPacket)<0)
			break;

		if(pPacket->stream_index == videoIndex){		//��Ƶpacket
			cout<<"decode thread"<<endl;
			avcodec_decode_video2(pCodecCtx_video,pFrame,&gotPicture,pPacket);		//����pCodecCtx,��packet���뵽pFrame��
			
			if(gotPicture){		//��ʾ������һ֡��pFrame��
				while(buff->isFull() && pkt->isDecode());
				buff->push(pFrame->data[0],pFrame->data[1],pFrame->data[2],pFrame->pkt_pts);
			}
		}else if(pPacket->stream_index == audioIndex){	//��Ƶpacket

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
	thread = SDL_CreateThread(decode,"decode",this);	//��ʼ�����߳�
}

void Codec::endDecode(){
	decode_flag = false;			//֪ͨ�����߳̽���
	SDL_WaitThread(thread,NULL);	//�ȴ������߳̽���
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