#include "GameLib/Framework.h"
using namespace GameLib;

#include "State.h"
#include "File.h"

void mainLoop();

//ȫ�ֱ���
State* gState = 0;

namespace GameLib {
	void Framework::update() {
		mainLoop();
	}
}

void mainLoop() {
	//�̶�֡�ʴ���
	Framework f = Framework::instance();
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
		File file("1.txt");
		if (!(file.getData())) { //û�����ݣ�
			cout << "stage file could not be read." << endl;
			return;
		}
		gState = new State(file.getData(), file.getSize());
	}

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