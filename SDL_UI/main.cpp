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
//	ShowWindow(GetForegroundWindow(),SW_HIDE);		//����cmd����

	SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO|SDL_INIT_TIMER);		//ע��SDL

	LJ_Window win("XiaoLiang MediaPlayer",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,1280,720);

 	LJ_VideoPlane videoPlane(&win,0,0,0,0);

// 	LJ_lable list(&win,0,0,50,50);
//  	list.setPic("list.bmp");

	win = win + videoPlane /*+ list*/;

	videoPlane.openMedia("F:\\movie\\��ʬ����.The.Walking.Dead.S10E02.720p-������Ļ��.mp4");

	SDL_Event event;

	while(1){
		SDL_WaitEvent(&event);

		switch(event.type){

		case SDL_MOUSEMOTION:	//����ƶ�
			break;

		case SDL_MOUSEBUTTONUP:	//�����
// 			for(auto it = win.getObjs().begin();it != win.getObjs().end();it++)
// 				if((*it)->isIn(event.motion.x,event.motion.y))
// 					cout<<"�������"<<endl;
			break;

		case SDL_KEYUP:			//����
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
													videoPlane.setRefresh(!videoPlane.isRefreshing());	//����/��ͣ
													break;
												}

			break;

		case SDL_DROPFILE:		//��ק�ļ���������
			if(videoPlane.isMediaOpened())		//�ر�֮ǰ�򿪵�
				videoPlane.closeMedia();
			videoPlane.openMedia(event.drop.file);	//�����ļ�
			break;

		case SDL_WINDOWEVENT:

			switch(event.window.event == SDL_WINDOWEVENT_MAXIMIZED){
				cout<<"*****************************************************************\n";
			}

			break;

		case SDL_QUIT:			//����X
			goto flag;
			break;

		}

	}flag:


	SDL_Quit();


	return 0;
}