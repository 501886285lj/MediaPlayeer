#include <iostream>
#include <Windows.h>
#include "LJ_SDL.h"
#include "Frame.h"
extern "C"{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
};

using namespace std;

int main(){
//	ShowWindow(GetForegroundWindow(),SW_HIDE);		//隐藏cmd窗口

	SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO|SDL_INIT_TIMER);		//注册SDL

	LJ_Window win("XiaoLiang MediaPlayer",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,1280,720);  //创建win对象

 	LJ_VideoPlane videoPlane(&win,0,0,0,0);        //创建视频面板

// 	LJ_lable list(&win,0,0,50,50);
//  	list.setPic("list.bmp");

	win = win + videoPlane /*+ list*/;     //将videoPlane放置到win中

	SDL_Event event;

	while(1){
		SDL_WaitEvent(&event);

		switch(event.type){

		case SDL_MOUSEMOTION:	//鼠标移动
			break;

		case SDL_MOUSEBUTTONUP:	//鼠标点击
// 			for(auto it = win.getObjs().end()-1;it >= win.getObjs().begin();it--)		//从后向前搜索到第一个点击到的对象，对象越在后面，则在窗体中越在表面
// 				if((*it)->isIn(event.motion.x,event.motion.y))
// 					cout<<"点击到了"<<endl;
			break;

		case SDL_KEYUP:			//键盘
												switch(event.key.keysym.sym){
												case SDLK_UP:
												//	videoPlane.setFps(videoPlane.getFps().num * 2,videoPlane.getFps().den);
							//						videoPlane.setAlpha(videoPlane.getAlpha()+20);
													videoPlane.setFps(videoPlane.getFps().num*2,videoPlane.getFps().den);
													break;

												case SDLK_DOWN:
												//	videoPlane.setFps(videoPlane.getFps().num / 2,videoPlane.getFps().den);
								//				videoPlane.setAlpha(videoPlane.getAlpha()-20);
													videoPlane.setFps(videoPlane.getFps().num/2,videoPlane.getFps().den);
													break;

												case SDLK_LEFT:
													videoPlane.backward(300);
													break;

												case SDLK_RIGHT:
													videoPlane.forward(300);
													break;

												case SDLK_ESCAPE:
													goto flag;
													break;

												case SDLK_SPACE:
								//					videoPlane.setVisiable(!videoPlane.isVisiable());
 								//					win.setBorder(false);
// 													win.setFullScreen(true);
//													videoPlane.closeMedia();
													videoPlane.setRefresh(!videoPlane.isRefreshing());	//播放/暂停
													break;
												}

			break;

		case SDL_DROPFILE:		//拖拽文件到窗口内
			if(videoPlane.isMediaOpened())		//关闭之前打开的
				videoPlane.closeMedia();
			videoPlane.openMedia(event.drop.file);	//打开新媒体
			break;

		case SDL_WINDOWEVENT:

			switch(event.window.event == SDL_WINDOWEVENT_MAXIMIZED){
				cout<<"*****************************************************************\n";
			}

			break;

		case SDL_QUIT:			//窗口X
			goto flag;
			break;

		}

	}flag:


	SDL_Quit();


	return 0;
}
