#include "State.h"
#include "Image.h"

#define GSIZE GAME_GRID_SIZE

class State::Object {
public:
	enum Type {
		OBJ_SPACE,
		OBJ_WALL,
		OBJ_CHESS,

		OBJ_UNKNOW
	};

	//图片ID
	enum ImageID {
		IMG_ID_CHESS,
		IMG_ID_WALL,
		IMG_ID_SPACE0,
		IMG_ID_SPACE1,
		IMG_ID_SPACE,
	};

	Object() :mType(OBJ_SPACE), mMoveX(0), mMoveY(0), mX(0), mY(0) {
	}

	//用舞台数据初始化自己
	void set(char c) {
		switch (c) {
		case '#': mType = OBJ_WALL; break;
		case ' ': mType = OBJ_SPACE; break;
		case 'c': mType = OBJ_CHESS; break;
		}
	}

	//绘画背景
	void drawBackground(int x, int y, Image* image) const {
		ImageID id = IMG_ID_SPACE;
		if (mType == IMG_ID_WALL) {//如果是墙，直接画
			drawCell(x, y, IMG_ID_WALL, image);
		}
		else {//其他情况，直接画space在背景上
			drawCell(x, y, IMG_ID_SPACE, image);
		}
	}
	//绘画可移动的部分
	void drawForeground(int x, int y, Image* image, int moveCount) const {
		//只有space和wall以外的物体可以移动
		ImageID id = IMG_ID_SPACE;
		if (mType == OBJ_CHESS) {
			id = IMG_ID_CHESS;
		}
		if (id != IMG_ID_SPACE && id != IMG_ID_WALL) {
			//计算移动
			int dx = mMoveX * (GSIZE - moveCount);
			int dy = mMoveY * (GSIZE - moveCount);
			image->draw(x * GSIZE - dx, y * GSIZE - dy, id * GSIZE, 0, GSIZE, GSIZE);
		}
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
	Type mType;

	//当前位置
	int mX;
	int mY;
	//当前移动
	int mMoveX;
	int mMoveY;
};

State::State(const char* stageData, int size) {
	setObjects(stageData, size);
	mObjects.setSize(mWidth, mHeight);
	//初始化舞台
	int x = 0;
	int y = 0;
	for (int i = 0; i < size; ++i) {
		Object t;
		switch (stageData[i]) {
		case '#': case ' ': case 'o': case 'O':
		case '.': case 'p': case 'P':
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
			//case '#': case ' ': case 'o': case 'O':
			//case '.': case 'p': case 'P':
			//	++x;
			//	break;
		case '\n':
			++y;
			//读到换行符时，最大值更新
			mWidth = (mWidth > x) ? mWidth : x;
			mHeight = (mHeight > y) ? mHeight : y;
			x = 0;
			break;
		default:
			//读到非换行符时，++x
			//注意: 有风险！
			++x;
			break;
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
}

//ver1.0一个棋子走 
//每次移动16帧，每帧1ms，固定刷新率60
void State::update(int dx, int dy) {
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
	int x, y;
	bool found = false;
	for (y = 0; y < mHeight; ++y) {
		for (x = 0; x < mWidth; ++x) {
			if (o(x, y).mType == Object::OBJ_CHESS) {
				found = true;
				break;
			}
		}
		if (found) {
			break;
		}
	}
	//移动
	//运动后坐标
	int tx = x + dx;
	int ty = y + dy;
	//检查最大和最小坐标。
	if (tx < 0 || ty < 0 || tx >= w || ty >= h) {
		return;
	}
	//移动棋子,如果方向上下一个格子为空白
	if (o(tx, ty).mType == Object::OBJ_SPACE) {
		o(tx, ty).move(dx, dy, Object::OBJ_CHESS);
		o(x, y).move(dx, dy, Object::OBJ_SPACE);
		mMoveCount = 1; //开始行动
	}

}