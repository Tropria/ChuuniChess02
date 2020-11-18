#include "State.h"
#include "Image.h"
#include "GameLib/Framework.h"
#define GSIZE GAME_GRID_SIZE

class State::Object {
public:
	enum Type {
		OBJ_SPACE,
		OBJ_WALL,
		OBJ_CHESS_MAN,
		OBJ_CHESS_BLOCK,
		OBJ_CURSOR,

		OBJ_UNKNOW
	};

	//图片ID 对应dds文件
	enum ImageID {
		IMG_ID_CHESS_MAN,
		IMG_ID_WALL,
		IMG_ID_CURSOR,
		IMG_ID_CHESS_BLOCK,
		IMG_ID_SPACE,
	};

	Object() :mType(OBJ_SPACE), mMoveX(0), mMoveY(0), isCursor(false) {
	}

	//用舞台数据初始化自己
	void set(char c) {
		switch (c) {
		case '#': mType = OBJ_WALL; break;
		case ' ': mType = OBJ_SPACE; break;
		case 'c': mType = OBJ_CHESS_MAN; break;
		case 'b': mType = OBJ_CHESS_BLOCK; break;
		case 'x': mType = OBJ_CURSOR; isCursor = true; break;
		}
	}

	//绘画背景
	void drawBackground(int x, int y, Image* image) const {
		if (mType == OBJ_WALL) {//如果是墙，直接画
			drawCell(x, y, IMG_ID_WALL, image);
		}else {//其他情况，直接画space在背景上
			drawCell(x, y, IMG_ID_SPACE, image);
		}
	}
	//绘画可移动的部分
	void drawForeground(int x, int y, Image* image, int moveCount) const {
		//只有space和wall以外的物体可以移动
		ImageID id = IMG_ID_SPACE;
		if (mType == OBJ_CHESS_MAN) {
			id = IMG_ID_CHESS_MAN;
		}
		else if (mType == OBJ_CHESS_BLOCK) {
			id = IMG_ID_CHESS_BLOCK;
		}
		if (id == IMG_ID_CHESS_MAN || id == IMG_ID_CHESS_BLOCK) {
			//计算移动
			int dx = mMoveX * (GSIZE - moveCount);
			int dy = mMoveY * (GSIZE - moveCount);
			image->draw(x * GSIZE - dx, y * GSIZE - dy, id * GSIZE, 0, GSIZE, GSIZE);
		}

		
	}
	
	//如果是cursor的话，最后再画cursor
	void drawCursor(int x, int y, Image* image, int moveCount) const {
			int dx = mMoveX * (GSIZE - moveCount);
			int dy = mMoveY * (GSIZE - moveCount);
			image->draw(x * GSIZE - dx, y * GSIZE - dy, IMG_ID_CURSOR * GSIZE, 0, GSIZE, GSIZE);
	}

	//画格子
	static void drawCell(
		int x,
		int y,
		int id,
		const Image* image) {
		image->draw(x * GSIZE, y * GSIZE, id * GSIZE, 0, GSIZE, GSIZE);
	}

	//设置移动
	void move(int dx, int dy, Type replaced) {
		mMoveX = dx;
		mMoveY = dy;
		mType = replaced;
	}
	void moveCursor(int dx, int dy) {
		mMoveX = dx;
		mMoveY = dy;
		isCursor = true;
	}

	Type getTypeFromChess(ChessType ct) {
		Type res = OBJ_CHESS_MAN;
		switch (ct)  {
			case MAN:	res = OBJ_CHESS_MAN;	break;
			case BLOCK:	res = OBJ_CHESS_BLOCK;	break;
		}
		return res;
	}

	Type mType;

	//当前位置
	bool isCursor;
	//当前移动
	int mMoveX;
	int mMoveY;
};

State::State(const char* stageData, int size):
mImage(0),
mMoveCount(0),
mWidth(0),
mHeight(0),
mCursor(0),
x_cursor(0),
y_cursor(0)
{
	setObjects(stageData, size);
	mObjects.setSize(mWidth, mHeight);
	//初始化舞台
	mCursor = new Object();
	mCursor->set('x');
	int x = 0;
	int y = 0;
	for (int i = 0; i < size; ++i) {
		Object t;
		switch (stageData[i]) {
		case '#': case ' ': case 'c': case 'b':
			mObjects(x, y).set(stageData[i]);
			++x;
			break;
		case '\n': x = 0; ++y; break; //换行处理
		}
	}
	//图片载入
	mImage = new Image("nimotsuKunImage2_16bit.dds");
}

//从文件中，根据换行符的个数读取出mWidth和mHeight的值
void State::setObjects(const char* stageData, int size) {
	const char* d = stageData;
	mWidth = mHeight = 0;
	int x = 0;
	int y = 0;
	for (int i = 0; i < size; ++i) {
		switch (stageData[i]) {
		case '#': case ' ': case 'c': case 'b':
			++x;
			break;
		case '\n':
			++y;
			//读到换行符时，最大值更新
			mWidth = (mWidth > x) ? mWidth : x;
			mHeight = (mHeight > y) ? mHeight : y;
			x = 0;
			break;
		//default:
		//	//读到非换行符时，++x
		//	//注意: 有风险！
		//	++x;
		//	break;
		}
	}
}

void State::draw() const {
	//分两步绘制。首先，绘制背景。
	for (int y = 0; y < mHeight; ++y) {
		for (int x = 0; x < mWidth; ++x) {
			mObjects(x, y).drawBackground(x, y, mImage);
		}
	}
	//接下来绘制前景，（可移动的部分）
	for (int y = 0; y < mHeight; ++y) {
		for (int x = 0; x < mWidth; ++x) {
			mObjects(x, y).drawForeground(x, y, mImage, mMoveCount);
		}
	}
	//最后我们画cursor
	mCursor->drawCursor(x_cursor, y_cursor, mImage, mMoveCount);
	for (int y = 0; y < mHeight; ++y) {
		for (int x = 0; x < mWidth; ++x) {
			//mObjects(x, y).drawCursor(x, y, mImage, mMoveCount);
		}
	}

	//mImage->draw(x_cursor * GSIZE, y_cursor * GSIZE, Object::IMG_ID_CURSOR * GSIZE, 0, GSIZE, GSIZE);
}


//ver1.0一个棋子走 
//ver1.1 两个棋子交替走
//每次移动16帧，每帧1ms，固定刷新率60
void State::update(int dx, int dy, bool isJ) {
	//如果移动计数达到16 
	if (mMoveCount == 16) {
		mMoveCount = 0; //
		//初始化移动
		for (int y = 0; y < mHeight; ++y) {
			for (int x = 0; x < mWidth; ++x) {
				mObjects(x, y).mMoveX = 0;
				mObjects(x, y).mMoveY = 0;
			}
		}
	}
	//移动时请勿更新。
	if (mMoveCount > 0) {
		++mMoveCount;
		return;
	}

	//使用简短的变量名。
	int w = mWidth;
	int h = mHeight;
	Array2D< Object >& o = mObjects;
	//查找棋子坐标
	/*int x, y;
	bool found = false;
	for (y = 0; y < mHeight; ++y) {
		for (x = 0; x < mWidth; ++x) {
			if (o(x, y).mType == Object::OBJ_CHESS_MAN) {
				found = true;
				break;
			}
		}
		if (found) {
			break;
		}
	}*/

	int x = x_cursor;
	int y = y_cursor;
	//移动cursor
	//运动后坐标
	int tx = x + dx;
	int ty = y + dy;
	//检查最大和最小坐标。
	if (tx < 0 || ty < 0 || tx >= w || ty >= h) {
		return;
	}
	x_cursor = tx;
	y_cursor = ty;
	//移动cursor
	if (dx != 0 || dy != 0) { 
		mCursor->moveCursor(dx, dy);
		o(tx, ty).isCursor = true;
		o(x, y).isCursor = false;
		mMoveCount = 1; 
	}
	//移动棋子,如果方向上下一个格子为空白

	//if (o(tx, ty).mType == Object::OBJ_SPACE) {
	//	o(tx, ty).move(dx, dy, Object::OBJ_CHESS_MAN);
	//	o(x, y).move(dx, dy, Object::OBJ_SPACE);
	//	mMoveCount = 1; //开始行动
	//}

}

void State::output() const {
	using namespace GameLib;
	cout <<"size0 : "<<mObjects.getSize0() << " size1 :" << mObjects.getSize1() << endl;
	for (int y = 0; y < mHeight; ++y) {
		for (int x = 0; x < mWidth; ++x) {
			cout << mObjects(x, y).mType <<' ';
		}
		cout << endl;
	}
}

