#include "LJ_SDL.h"

LJ_lable::LJ_lable(LJ_Window *win,int x,int y,int w,int h):LJ_object(win,x,y,w,h){
}

LJ_lable::~LJ_lable(){
}

void LJ_lable::setPic(char* route){
	if(*texture)
		SDL_DestroyTexture(*texture);
	*texture = SDL_CreateTextureFromSurface(win->getRenderer(),SDL_LoadBMP(route));
}