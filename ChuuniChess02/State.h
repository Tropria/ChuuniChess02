#pragma once
#ifndef INCLUDED_STATE_H
#define INCLUDED_STATE_H

#define GAME_GRID_SIZE (16)

#include "Array2D.h"

class Image;
enum ChessType {
	MAN,
	BLOCK,
};

class State {
public:
	State(const char* stageData, int size);
	void update(int dx, int dy, bool isJ);
	void draw() const;

	int mWidth;//地图尺寸
	int mHeight;
	void output() const ;
private:
	class Object;
	void setObjects(const char* stageData, int size);

	Array2D<Object> mObjects;

	ChessType curColor;//当前行动的一方的颜色 0 white 1 black
	Object* mCursor;
	int x_cursor;
	int y_cursor;

	Image* mImage;

	int mMoveCount;

};

#endif