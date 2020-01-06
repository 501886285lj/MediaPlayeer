#include "LJ_SDL.h"
#include <iostream>


LJ_object::LJ_object(LJ_Window *win,int x,int y,int w,int h):win(win)
{
	position.x = x;
	position.y = y;
	position.w = w;
	position.h = h;

	visiable = true;

	texture = new SDL_Texture *;	//创建及初始化*texture
	*texture = NULL;

	texture_mutex = SDL_CreateMutex();
}


LJ_object::~LJ_object(void)
{
	(*win) -= (*this);		//解除和window的关系
	SDL_DestroyMutex(texture_mutex);
	delete texture;
}

void LJ_object::setWindow(LJ_Window *win){
	this->win = win;
}

void LJ_object::setVisiable(bool visiable){
	this->visiable = visiable;
}

void LJ_object::setAlpha(uint8_t alpha){
	alpha = (alpha<=255?alpha:255);
	alpha = (alpha>=0?alpha:0);

	SDL_SetTextureAlphaMod(*texture,alpha);
}

void LJ_object::initTexture(){
	 	int Ysize = position.w * position.h;
	 	int UVsize = Ysize/4;
	 	uint8_t *yy = new uint8_t[Ysize];
	 	uint8_t *uu = new uint8_t[UVsize];
	 	uint8_t *vv = new uint8_t[UVsize];
	  	memset(yy,0,Ysize);
	  	memset(uu,0,UVsize);
	  	memset(vv,0,UVsize);
	  	updateTexture(yy,uu,vv);
		delete[] yy,uu,vv;
}

void LJ_object::lockTexture(){
	SDL_LockMutex(texture_mutex);
}

void LJ_object::unlockTexture(){
	SDL_UnlockMutex(texture_mutex);
}

void LJ_object::updateTexture(uint8_t* Y,uint8_t* U,uint8_t* V){
 	SDL_UpdateYUVTexture(*texture,&position,Y,position.w,U,position.w/2,V,position.w/2);
}

bool LJ_object::isVisiable(){
	return visiable;
}

SDL_Texture* LJ_object::getTexture() const{
	return *texture;
}

SDL_Rect& LJ_object::getPosition(){
	return position;
}

uint8_t LJ_object::getAlpha(){
	uint8_t alpha;
	SDL_GetTextureAlphaMod(*texture,&alpha);
	return alpha;
}

bool LJ_object::isIn(int x,int y){
	if(x>=position.x  && x<position.x + position.w && y>=position.y && y<position.y+position.h)
		return true;
	return false;
}

void LJ_object::resize(int w,int h){
	position.w = w;
	position.h = h;
}