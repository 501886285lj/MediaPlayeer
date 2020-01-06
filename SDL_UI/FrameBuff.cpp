#include "Frame.h"
#include <iostream>

using namespace std;

FrameBuff::FrameBuff(int width,int height,int capacity):width(width),height(height),capacity(capacity)
{
	//队列头尾
	head = 0;
	tail = 0;
	
	buff = new Frame[capacity];
	for(int i = 0;i<capacity;i++)
		buff[i].alloc(width,height);

}


FrameBuff::~FrameBuff(void)
{
// 	while(!isEmpty())	//先全部出队列
// 		pop();

	clear();

	for(int i = 0;i<capacity;i++)	//再进行内存释放
		buff[i].free();

	delete[] buff;
}

int FrameBuff::size(){

	if(tail < head)
		return tail+capacity-head;
	else
		return tail-head;
}

bool FrameBuff::isEmpty(){
	return (tail == head);
}

bool FrameBuff::isFull(){
	return (((tail+1) % capacity) == head);
}

int FrameBuff::getHeight(){
	return height;
}

int FrameBuff::getWidth(){
	return width;
}

int FrameBuff::push(const uint8_t *Y,const uint8_t *U,const uint8_t *V,const int64_t pts){
	if(isFull())
		return -1;

	buff[tail].setY(Y);
	buff[tail].setU(U);
	buff[tail].setV(V);

	buff[tail].setPts(pts);

	tail = (tail+1)%capacity;

	return 0;
}

Frame* FrameBuff::top(){
	if(isEmpty())
		return NULL;

	return buff+head;
}

int FrameBuff::pop(){
	if(isEmpty())
		return -1;

	head = (head+1)%capacity;
	return 0;
}

void FrameBuff::clear(){
	head = 0;
	tail = 0;
}