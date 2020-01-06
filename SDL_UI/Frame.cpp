#include "Frame.h"


Frame::Frame(int w,int h)
{
	alloc(w,h);
}

Frame::Frame(){
}

void Frame::alloc(int w,int h){
	width = w;
	height = h;

	Yplane = new uint8_t[w*h];
	Uplane = new uint8_t[w*h/4];
	Vplane = new uint8_t[w*h/4];

	memset(Yplane,0,w*h);
	memset(Uplane,0,w*h/4);
	memset(Vplane,0,w*h/4);
}

void Frame::free(){
	if(Yplane)
		delete[] Yplane;
	if(Uplane)
		delete[] Uplane;
	if(Vplane)
		delete[] Vplane;
}

Frame::~Frame(void)
{
}

uint8_t* Frame::getY(){
	return Yplane;
}

uint8_t* Frame::getU(){
	return Uplane;
}

uint8_t* Frame::getV(){
	return Vplane;
}

int Frame::getHeight(){
	return height;
}

int Frame::getWidth(){
	return width;
}

void Frame::setY(const uint8_t *Y){
	int size = height * width;
	for(int i = 0;i<size;i++)
		Yplane[i] = Y[i];
}

void Frame::setV(const uint8_t *V){
	int size = height * width / 4;
	for(int i = 0 ;i<size;i++)
		Vplane[i] = V[i];
}

void Frame::setU(const uint8_t *U){
	int size = height * width / 4;
	for(int i = 0 ;i<size;i++)
		Uplane[i] = U[i];
}

void Frame::setPts(int64_t pts){
	this->pts = pts;
}

int64_t Frame::getPts(){
	return pts;
}