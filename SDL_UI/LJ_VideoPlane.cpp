#include "LJ_SDL.h"

LJ_VideoPlane::LJ_VideoPlane(LJ_Window *win,int x,int y,int w,int h):LJ_object(win,x,y,w,h){
	buff = new FrameBuff *;
	*buff = NULL;
	
	setFps(24,1);		//��ʼ��֡��

	*texture = SDL_CreateTexture(win->getRenderer(),SDL_PIXELFORMAT_IYUV,SDL_TEXTUREACCESS_STREAMING,w,h);	//����YUV����

	refresh_flag = true;
	run_flag = true;
 	thread = SDL_CreateThread(refresh,"refresh_thread",this);		//��ʼrefresh�߳�

	codec = new Codec();		//����������
}

LJ_VideoPlane::~LJ_VideoPlane(){
	if(isMediaOpened())
		closeMedia();

	run_flag = false;
	SDL_WaitThread(thread,NULL);		//�ȴ�refresh�߳�

	SDL_DestroyTexture(*texture);

	delete codec;	//�ͷŽ�����

	delete buff;
}

bool LJ_VideoPlane::isRunning(){
	return run_flag;
}

bool LJ_VideoPlane::isMediaOpened(){
	if(codec->getFormatCtx())
		return true;
	else
		return false;
}

void LJ_VideoPlane::setFps(int num,int den){
	fps.num = num;
	fps.den = den;
}

AVRational LJ_VideoPlane::getFps(){
	return fps;
}

int LJ_VideoPlane::refresh(void *data){
	LJ_VideoPlane* vp = (LJ_VideoPlane*)data;
	FrameBuff *&buff = *(vp->buff);

	while(vp->isRunning()){
		if(!vp->isRefreshing())
			continue;
		cout<<"refresh thread!"<<endl;
			if(buff && !buff->isEmpty()){
				vp->updateTexture(buff->top()->getY(),buff->top()->getU(),buff->top()->getV());
				buff->pop();
			}
		SDL_Delay(vp->getFps().den * 1000 / vp->getFps().num);
	}
	cout<<"refresh thread enddddddd!"<<endl;
	return 0;
}

int LJ_VideoPlane::openMedia(char* route){
	codec->openMedia(route);		//��media����ȡ��Ϣ
	setFps(codec->getCodecCtx_video()->framerate.num,codec->getCodecCtx_video()->framerate.den);		//����֡��
	win->resize(codec->getCodecCtx_video()->width,codec->getCodecCtx_video()->height);					//���ô��ڴ�С
	resize(codec->getCodecCtx_video()->width,codec->getCodecCtx_video()->height);						//����videoplane��С
	*buff = new FrameBuff(codec->getCodecCtx_video()->width,codec->getCodecCtx_video()->height,20);		//����buff
	codec->startDecode((*buff));			//���뵽buff��
	return 0;
}

int LJ_VideoPlane::closeMedia(){
	codec->endDecode();		//��������
	delete (*buff);			//ɾ��buff
	codec->closeMedia();	//�ر�media
	return 0;
}

void LJ_VideoPlane::resize(int w,int h){
	__super::resize(w, h);

  	setVisiable(false);	//�Ȳ��ɼ����ж�paint�̵߳�ʹ��
  	setRefresh(false);	//ֹͣ���£��ж�refresh�̵߳�ʹ��
	SDL_DestroyTexture(*texture);	//�ٴݻ�
	*texture = SDL_CreateTexture(win->getRenderer(),SDL_PIXELFORMAT_IYUV,SDL_TEXTUREACCESS_STREAMING,w,h);	//���ؽ�
  	setRefresh(true);	//��ʼ����
  	setVisiable(true);	//�ɼ�
}

void LJ_VideoPlane::setRefresh(bool refresh_flag){
	this->refresh_flag = refresh_flag;
}

bool LJ_VideoPlane::isRefreshing(){
	return refresh_flag;
}

void LJ_VideoPlane::forward(int sec){
	codec->forward(sec);
	(*buff)->clear();		//���buff����ֹ�ӳ�
}

void LJ_VideoPlane::backward(int sec){
	codec->backward(sec);
	(*buff)->clear();	//���buff����ֹ�ӳ�
}