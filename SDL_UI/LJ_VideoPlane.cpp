#include "LJ_SDL.h"

LJ_VideoPlane::LJ_VideoPlane(LJ_Window *win,int x,int y,int w,int h):LJ_object(win,x,y,w,h){
	buff = new FrameBuff *;
	*buff = NULL;
	
	setFps(24,1);		//初始化帧率

	*texture = SDL_CreateTexture(win->getRenderer(),SDL_PIXELFORMAT_IYUV,SDL_TEXTUREACCESS_STREAMING,w,h);	//创建YUV纹理

	refresh_flag = true;
	run_flag = true;
 	thread = SDL_CreateThread(refresh,"refresh_thread",this);		//开始refresh线程

	codec = new Codec();		//创建解码器
}

LJ_VideoPlane::~LJ_VideoPlane(){
	if(isMediaOpened())
		closeMedia();

	run_flag = false;
	SDL_WaitThread(thread,NULL);		//等待refresh线程

	SDL_DestroyTexture(*texture);

	delete codec;	//释放解码器

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
	codec->openMedia(route);		//打开media，获取信息
	setFps(codec->getCodecCtx_video()->framerate.num,codec->getCodecCtx_video()->framerate.den);		//设置帧率
	win->resize(codec->getCodecCtx_video()->width,codec->getCodecCtx_video()->height);					//设置窗口大小
	resize(codec->getCodecCtx_video()->width,codec->getCodecCtx_video()->height);						//设置videoplane大小
	*buff = new FrameBuff(codec->getCodecCtx_video()->width,codec->getCodecCtx_video()->height,20);		//建立buff
	codec->startDecode((*buff));			//解码到buff中
	return 0;
}

int LJ_VideoPlane::closeMedia(){
	codec->endDecode();		//结束编码
	delete (*buff);			//删除buff
	codec->closeMedia();	//关闭media
	return 0;
}

void LJ_VideoPlane::resize(int w,int h){
	__super::resize(w, h);

  	setVisiable(false);	//先不可见，中断paint线程的使用
  	setRefresh(false);	//停止更新，中断refresh线程的使用
	SDL_DestroyTexture(*texture);	//再摧毁
	*texture = SDL_CreateTexture(win->getRenderer(),SDL_PIXELFORMAT_IYUV,SDL_TEXTUREACCESS_STREAMING,w,h);	//再重建
  	setRefresh(true);	//开始更新
  	setVisiable(true);	//可见
}

void LJ_VideoPlane::setRefresh(bool refresh_flag){
	this->refresh_flag = refresh_flag;
}

bool LJ_VideoPlane::isRefreshing(){
	return refresh_flag;
}

void LJ_VideoPlane::forward(int sec){
	codec->forward(sec);
	(*buff)->clear();		//清除buff，防止延迟
}

void LJ_VideoPlane::backward(int sec){
	codec->backward(sec);
	(*buff)->clear();	//清除buff，防止延迟
}