#pragma once

#include <vector>
#include <iostream>
#include "Codec.h"
#include "Frame.h"
extern "C"{
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_thread.h>
#include <SDL_render.h>
#undef main
};

using namespace std;



class LJ_object;
class LJ_Window;
class LJ_VideoPlane;


class LJ_Window
{
	SDL_Window *win;
	SDL_Renderer **render;
	SDL_Rect position;
	vector<LJ_object*> objs;
	SDL_Thread *paint_thread;
	bool run_flag;
	bool paint_flag;

public:

	LJ_Window(char *title,int x,int y,int w,int h);
	~LJ_Window(void);

	SDL_Window* getWindows();
	SDL_Renderer* getRenderer();
	vector<LJ_object*> &getObjs();
	bool isBorder();
	bool isPainting();

	void resize(int w,int h);

	void clearObjs();

	LJ_Window& operator+(LJ_object& obj);
	LJ_Window& operator-(LJ_object& obj);
	LJ_Window& operator+=(LJ_object& obj);
	LJ_Window& operator-=(LJ_object& obj);

	void setBorder(bool border);
	void setFullScreen(bool fullScreen);
	void setPosition(int x,int y);

	void setPaint(bool paint_flag);

	static int paint(void *data);
	
	void show();
};



/*
*	SDL基类对象
*/
class LJ_object
{

protected:
	LJ_Window *win;
	SDL_Texture **texture;
	SDL_Rect position;
	bool visiable;
	SDL_mutex *texture_mutex;

public:

	LJ_object(LJ_Window *win,int x,int y,int w,int h);
	~LJ_object(void);

	void setWindow(LJ_Window *win);
	void setVisiable(bool visiable);
	void setAlpha(uint8_t alpha);

	void initTexture();

	void lockTexture();
	void unlockTexture();

	void updateTexture(uint8_t* Y,uint8_t* U,uint8_t* V);

	bool isVisiable();

	void resize(int w,int h);

	SDL_Texture* getTexture() const;
	SDL_Rect& getPosition();
	uint8_t getAlpha();

	bool isIn(int x,int y);

};


/*
*	视频面板对象
*/
class LJ_VideoPlane:public LJ_object{
	FrameBuff **buff;
	SDL_Thread *thread;
	bool run_flag;
	AVRational fps;
	Codec *codec;
	bool refresh_flag;

public:
	LJ_VideoPlane(LJ_Window *win,int x,int y,int w,int h);
	~LJ_VideoPlane();

	void setFps(int num ,int den);
	AVRational getFps();

	bool isRunning();
	bool isMediaOpened();

	void setRefresh(bool refresh_flag);
	bool isRefreshing();

	void resize(int w,int h);

	int openMedia(char* route);
	int closeMedia();

	void forward(int sec);
	void backward(int sec);

	static int refresh(void *data);
};


/*
*	lable对象
*/
class LJ_lable:public LJ_object{

public:

	LJ_lable(LJ_Window *win,int x,int y,int w,int h);
	~LJ_lable();

	void setPic(char* route);
	void setString(char* s);
};