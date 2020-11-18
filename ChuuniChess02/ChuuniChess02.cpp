#include "GameLib/Framework.h"
using namespace GameLib;

#include "State.h"
#include "File.h"

void mainLoop();

//ȫ�ֱ���
State* gState = 0;
const int gFrameInterval = 8; //16����62.5fps��
unsigned gPreviousTime[10]; //10��¼���ʱ��
int gCounter = 0; //һ�������������ڼ�����ѭ���Ĵ���

#define USE_SLEEP_FUNCTION //ʹ��sleep����

namespace GameLib {
	void Framework::update() {
		mainLoop();
	}
}

void mainLoop() {
	//�̶�֡�ʴ���
	Framework f = Framework::instance();
	while ((f.time() - gPreviousTime[9]) < gFrameInterval) {
#ifdef USE_SLEEP_FUNCTION
		f.sleep(1);
#endif
	}
	//֡�ʼ���
	unsigned currentTime = f.time();
	unsigned frameInterval10 = currentTime - gPreviousTime[0];
	if (gCounter % 60 == 0) { //60ÿ֡��ʾһ��֡��
		cout << "frameInterval:" << frameInterval10 / 10;
		cout << " FrameRate:" << 10 * 1000 / frameInterval10 << endl;
	}
	++gCounter;
	//������ʷ
	for (int i = 0; i < 9; ++i) {
		gPreviousTime[i] = gPreviousTime[i + 1];
	}
	gPreviousTime[9] = currentTime;


	//����ť����������
	if (Framework::instance().isEndRequested()) {
		if (gState) {
			delete gState;
			gState = 0;
		}
		return;
	}
	//��ʼ����һ֡
	if (!gState) {
		File file("stageData.txt");
		if (!(file.getData())) { //û�����ݣ�
			cout << "stage file could not be read." << endl;
			return;
		}
		gState = new State(file.getData(), file.getSize());
	}
//cout << "gState.width "<< gState->mWidth<< " height "<< gState->mHeight << endl;
//gState->output();

	//��ȡ����
	int dx = 0;
	int dy = 0;
	if (f.isKeyOn('a')) {
		dx -= 1;
	}
	else if (f.isKeyOn('d')) {
		dx += 1;
	}
	else if (f.isKeyOn('w')) {
		dy -= 1;
	}
	else if (f.isKeyOn('s')) {
		dy += 1;
	}
	//�����ж�
	if (f.isKeyOn('q')) {
		delete gState;
		gState = 0;
		f.requestEnd();
		return;
	}

	gState->update(dx, dy);

	gState->draw();

}

