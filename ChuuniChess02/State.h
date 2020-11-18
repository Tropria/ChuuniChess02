#pragma once
#ifndef INCLUDED_STATE_H
#define INCLUDED_STATE_H

#define GAME_GRID_SIZE (16)

#include "Array2D.h"

class Image;

class State {
public:
	State(const char* stageData, int size);
	void update(int dx, int dy);
	void draw() const;

	int mWidth;//µØÍ¼³ß´ç
	int mHeight;
	void output() const ;
private:
	class Object;
	void setObjects(const char* stageData, int size);

	Array2D<Object> mObjects;

	
	Image* mImage;

	int mMoveCount;
};

#endif