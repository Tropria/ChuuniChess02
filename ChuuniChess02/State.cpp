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

	//ͼƬID
	enum ImageID {
		IMG_ID_CHESS,
		IMG_ID_WALL,
		IMG_ID_SPACE0,
		IMG_ID_SPACE1,
		IMG_ID_SPACE,
	};

	Object() :mType(OBJ_SPACE), mMoveX(0), mMoveY(0), mX(0), mY(0) {
	}

	//����̨���ݳ�ʼ���Լ�
	void set(char c) {
		switch (c) {
		case '#': mType = OBJ_WALL; break;
		case ' ': mType = OBJ_SPACE; break;
		case 'c': mType = OBJ_CHESS; break;
		}
	}

	//�滭����
	void drawBackground(int x, int y, Image* image) const {
		ImageID id = IMG_ID_SPACE;
		if (mType == IMG_ID_WALL) {//�����ǽ��ֱ�ӻ�
			drawCell(x, y, IMG_ID_WALL, image);
		}
		else {//���������ֱ�ӻ�space�ڱ�����
			drawCell(x, y, IMG_ID_SPACE, image);
		}
	}
	//�滭���ƶ��Ĳ���
	void drawForeground(int x, int y, Image* image, int moveCount) const {
		//ֻ��space��wall�������������ƶ�
		ImageID id = IMG_ID_SPACE;
		if (mType == OBJ_CHESS) {
			id = IMG_ID_CHESS;
		}
		if (id != IMG_ID_SPACE && id != IMG_ID_WALL) {
			//�����ƶ�
			int dx = mMoveX * (GSIZE - moveCount);
			int dy = mMoveY * (GSIZE - moveCount);
			image->draw(x * GSIZE - dx, y * GSIZE - dy, id * GSIZE, 0, GSIZE, GSIZE);
		}
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
	Type mType;

	//��ǰλ��
	int mX;
	int mY;
	//��ǰ�ƶ�
	int mMoveX;
	int mMoveY;
};

State::State(const char* stageData, int size) {
	setObjects(stageData, size);
	mObjects.setSize(mWidth, mHeight);
	//��ʼ����̨
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
			//case '#': case ' ': case 'o': case 'O':
			//case '.': case 'p': case 'P':
			//	++x;
			//	break;
		case '\n':
			++y;
			//�������з�ʱ�����ֵ����
			mWidth = (mWidth > x) ? mWidth : x;
			mHeight = (mHeight > y) ? mHeight : y;
			x = 0;
			break;
		default:
			//�����ǻ��з�ʱ��++x
			//ע��: �з��գ�
			++x;
			break;
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
}

//ver1.0һ�������� 
//ÿ���ƶ�16֡��ÿ֡1ms���̶�ˢ����60
void State::update(int dx, int dy) {
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
	//�ƶ�
	//�˶�������
	int tx = x + dx;
	int ty = y + dy;
	//���������С���ꡣ
	if (tx < 0 || ty < 0 || tx >= w || ty >= h) {
		return;
	}
	//�ƶ�����,�����������һ������Ϊ�հ�
	if (o(tx, ty).mType == Object::OBJ_SPACE) {
		o(tx, ty).move(dx, dy, Object::OBJ_CHESS);
		o(x, y).move(dx, dy, Object::OBJ_SPACE);
		mMoveCount = 1; //��ʼ�ж�
	}

}