#pragma once
#include <queue>
#include <stdint.h>
#include <iostream>

using namespace std;

class Frame{
	int width;
	int height;
	uint8_t *Yplane;
	uint8_t *Uplane;
	uint8_t *Vplane;
	int64_t pts;

public:
	Frame(int w,int h);
	Frame();
	void alloc(int w,int h);
	void free();
	~Frame(void);

	void setY(const uint8_t *Y);
	void setU(const uint8_t *U);
	void setV(const uint8_t *V);

	void setPts(int64_t pks);

	uint8_t *getY();
	uint8_t *getU();
	uint8_t *getV();
	int64_t getPts();

	int getWidth();
	int getHeight();

};

class FrameBuff
{
	int height;
	int width;
	Frame* buff;
	int capacity;
	int head;
	int tail;

public:
	FrameBuff(int width,int height,int capacity);
	~FrameBuff(void);

	int getHeight();
	int getWidth();
	int size();	//得到队列内的帧数
	bool isEmpty();	//空？
	bool isFull();	//满？

	int push(const uint8_t *Y,const uint8_t *U,const uint8_t *V,const int64_t pks);
	Frame* top();
	int pop();

	void clear();
};

