#include "LJ_SDL.h"
#include <iostream>
#include <algorithm>

LJ_Window::LJ_Window(char *title,int x,int y,int w,int h)
{
	win = SDL_CreateWindow(title,x,y,w,h,SDL_WINDOW_RESIZABLE|SDL_WINDOW_SHOWN|SDL_WINDOW_OPENGL|SDL_WINDOW_MOUSE_CAPTURE);
	render = new SDL_Renderer *;
	*render = SDL_CreateRenderer(win,-1,SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);

	paint_flag = true;

	position.x = x;
	position.y = y;
	position.w = w;
	position.h = h;

	SDL_ShowWindow(win);

	run_flag = true;
	paint_thread = SDL_CreateThread(paint,"paint_thread",this);		//刷新线程
}

LJ_Window::~LJ_Window(void)
{
	run_flag = false;					//通知paint线程结束
	SDL_WaitThread(paint_thread,NULL);	//等待paint线程结束
	SDL_DestroyRenderer(*render);
	delete render;
	SDL_DestroyWindow(win);
}

LJ_Window& LJ_Window::operator+(LJ_object &obj){
	objs.push_back(&obj);	//将对象放在队列
	return *this;
}

LJ_Window& LJ_Window::operator-(LJ_object &obj){
	auto it = find(objs.begin(),objs.end(),&obj);
	objs.erase(it);
	return *this;
}

LJ_Window& LJ_Window::operator+=(LJ_object &obj){
	objs.push_back(&obj);	//将对象放在队列
	return *this;
}

LJ_Window& LJ_Window::operator-=(LJ_object &obj){
	auto it = find(objs.begin(),objs.end(),&obj);
	if(it != objs.end())
		objs.erase(it);
	return *this;
}

void LJ_Window::clearObjs(){
	objs.clear();
}

SDL_Window* LJ_Window::getWindows(){
	return win;
}

SDL_Renderer* LJ_Window::getRenderer(){
	return *render;
}

vector<LJ_object*> & LJ_Window::getObjs(){
	return objs;
}

bool LJ_Window::isBorder(){
	return !(SDL_GetWindowFlags(win) & SDL_WINDOW_BORDERLESS);
}

void LJ_Window::setBorder(bool border){
	if(border)
		SDL_SetWindowBordered(win,SDL_TRUE);
	else
		SDL_SetWindowBordered(win,SDL_FALSE);
}

void LJ_Window::setFullScreen(bool fullScreen){
	if(fullScreen)
		SDL_SetWindowFullscreen(win,SDL_WINDOW_FULLSCREEN);
	else
		SDL_SetWindowBordered(win,SDL_FALSE);
}

int LJ_Window::paint(void *data){
	LJ_Window *win = (LJ_Window *)data;

	while(win->run_flag){

		if(!win->isPainting())	//停止渲染
			continue;

		SDL_RenderClear(win->getRenderer());
		if(win->getObjs().size() > 0){
			cout<<"paint thread"<<endl;
			for(int i = 0;i<win->getObjs().size();i++)
				if(win->getObjs()[i]->isVisiable()){
					SDL_RenderCopy(win->getRenderer(),win->getObjs()[i]->getTexture(),nullptr,&win->getObjs()[i]->getPosition());
				}
 		} 
		SDL_Delay(5);
 		SDL_RenderPresent(win->getRenderer());
	}
	cout<<"paint thread endddddddd!"<<endl;
	return 0;
}

void LJ_Window::show(){
	SDL_RenderClear(*render);

	if(objs.size() > 0){
		for(int i = 0;i<objs.size();i++)
			if(objs[i]->isVisiable())
				SDL_RenderCopy(*render,objs[i]->getTexture(),nullptr,&objs[i]->getPosition());
	} 

	SDL_RenderPresent(*render);
}



void LJ_Window::resize(int w,int h){
	position.w = w;
	position.h = h;

	setPaint(false);
	SDL_Delay(800);
	SDL_SetWindowSize(win,w,h);
	SDL_SetWindowPosition(win,SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED);
	SDL_DestroyRenderer(*render);
	*render = SDL_CreateRenderer(win,-1,SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
	setPaint(true);

}

void LJ_Window::setPosition(int x,int y){
	SDL_SetWindowPosition(win,x,y);
}

bool LJ_Window::isPainting(){
	return paint_flag;
}

void LJ_Window::setPaint(bool paint_flag){
	this->paint_flag = paint_flag;
}