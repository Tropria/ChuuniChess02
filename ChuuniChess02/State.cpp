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

	//ͼƬID ��Ӧdds�ļ�
	enum ImageID {
		IMG_ID_CHESS_MAN,
		IMG_ID_WALL,
		IMG_ID_CURSOR,
		IMG_ID_CHESS_BLOCK,
		IMG_ID_SPACE,
	};

	Object() :mType(OBJ_SPACE), mMoveX(0), mMoveY(0), isCursor(false) {
	}

	//����̨���ݳ�ʼ���Լ�
	void set(char c) {
		switch (c) {
		case '#': mType = OBJ_WALL; break;
		case ' ': mType = OBJ_SPACE; break;
		case 'c': mType = OBJ_CHESS_MAN; break;
		case 'b': mType = OBJ_CHESS_BLOCK; break;
		case 'x': mType = OBJ_CURSOR; isCursor = true; break;
		}
	}

	//�滭����
	void drawBackground(int x, int y, Image* image) const {
		if (mType == OBJ_WALL) {//�����ǽ��ֱ�ӻ�
			drawCell(x, y, IMG_ID_WALL, image);
		}else {//���������ֱ�ӻ�space�ڱ�����
			drawCell(x, y, IMG_ID_SPACE, image);
		}
	}
	//�滭���ƶ��Ĳ���
	void drawForeground(int x, int y, Image* image, int moveCount) const {
		//ֻ��space��wall�������������ƶ�
		ImageID id = IMG_ID_SPACE;
		if (mType == OBJ_CHESS_MAN) {
			id = IMG_ID_CHESS_MAN;
		}
		else if (mType == OBJ_CHESS_BLOCK) {
			id = IMG_ID_CHESS_BLOCK;
		}
		if (id == IMG_ID_CHESS_MAN || id == IMG_ID_CHESS_BLOCK) {
			//�����ƶ�
			int dx = mMoveX * (GSIZE - moveCount);
			int dy = mMoveY * (GSIZE - moveCount);
			image->draw(x * GSIZE - dx, y * GSIZE - dy, id * GSIZE, 0, GSIZE, GSIZE);
		}

		
	}
	
	//�����cursor�Ļ�������ٻ�cursor
	void drawCursor(int x, int y, Image* image, int moveCount) const {
			int dx = mMoveX * (GSIZE - moveCount);
			int dy = mMoveY * (GSIZE - moveCount);
			image->draw(x * GSIZE - dx, y * GSIZE - dy, IMG_ID_CURSOR * GSIZE, 0, GSIZE, GSIZE);
	}

	//������
	static void drawCell(
		int x,
		int y,
		int id,
		const Image* image) {
		image->draw(x * GSIZE, y * GSIZE, id * GSIZE, 0, GSIZE, GSIZE);
	}

	//�����ƶ�
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

	//��ǰλ��
	bool isCursor;
	//��ǰ�ƶ�
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
	//��ʼ����̨
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
		case '\n': x = 0; ++y; break; //���д���
		}
	}
	//ͼƬ����
	mImage = new Image("nimotsuKunImage2_16bit.dds");
}

//���ļ��У����ݻ��з��ĸ�����ȡ��mWidth��mHeight��ֵ
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
			//�������з�ʱ�����ֵ����
			mWidth = (mWidth > x) ? mWidth : x;
			mHeight = (mHeight > y) ? mHeight : y;
			x = 0;
			break;
		//default:
		//	//�����ǻ��з�ʱ��++x
		//	//ע��: �з��գ�
		//	++x;
		//	break;
		}
	}
}

void State::draw() const {
	//���������ơ����ȣ����Ʊ�����
	for (int y = 0; y < mHeight; ++y) {
		for (int x = 0; x < mWidth; ++x) {
			mObjects(x, y).drawBackground(x, y, mImage);
		}
	}
	//����������ǰ���������ƶ��Ĳ��֣�
	for (int y = 0; y < mHeight; ++y) {
		for (int x = 0; x < mWidth; ++x) {
			mObjects(x, y).drawForeground(x, y, mImage, mMoveCount);
		}
	}
	//������ǻ�cursor
	mCursor->drawCursor(x_cursor, y_cursor, mImage, mMoveCount);
	for (int y = 0; y < mHeight; ++y) {
		for (int x = 0; x < mWidth; ++x) {
			//mObjects(x, y).drawCursor(x, y, mImage, mMoveCount);
		}
	}

	//mImage->draw(x_cursor * GSIZE, y_cursor * GSIZE, Object::IMG_ID_CURSOR * GSIZE, 0, GSIZE, GSIZE);
}


//ver1.0һ�������� 
//ver1.1 �������ӽ�����
//ÿ���ƶ�16֡��ÿ֡1ms���̶�ˢ����60
void State::update(int dx, int dy, bool isJ) {
	//����ƶ������ﵽ16 
	if (mMoveCount == 16) {
		mMoveCount = 0; //
		//��ʼ���ƶ�
		for (int y = 0; y < mHeight; ++y) {
			for (int x = 0; x < mWidth; ++x) {
				mObjects(x, y).mMoveX = 0;
				mObjects(x, y).mMoveY = 0;
			}
		}
	}
	//�ƶ�ʱ������¡�
	if (mMoveCount > 0) {
		++mMoveCount;
		return;
	}

	//ʹ�ü�̵ı�������
	int w = mWidth;
	int h = mHeight;
	Array2D< Object >& o = mObjects;
	//������������
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
	//�ƶ�cursor
	//�˶�������
	int tx = x + dx;
	int ty = y + dy;
	//���������С���ꡣ
	if (tx < 0 || ty < 0 || tx >= w || ty >= h) {
		return;
	}
	x_cursor = tx;
	y_cursor = ty;
	//�ƶ�cursor
	if (dx != 0 || dy != 0) { 
		mCursor->moveCursor(dx, dy);
		o(tx, ty).isCursor = true;
		o(x, y).isCursor = false;
		mMoveCount = 1; 
	}
	//�ƶ�����,�����������һ������Ϊ�հ�

	//if (o(tx, ty).mType == Object::OBJ_SPACE) {
	//	o(tx, ty).move(dx, dy, Object::OBJ_CHESS_MAN);
	//	o(x, y).move(dx, dy, Object::OBJ_SPACE);
	//	mMoveCount = 1; //��ʼ�ж�
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

