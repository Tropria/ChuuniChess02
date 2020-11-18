#include "GameLib/Framework.h"
using namespace GameLib;

#include "State.h"
#include "File.h"

void mainLoop();

//全局变量
State* gState = 0;

namespace GameLib {
	void Framework::update() {
		mainLoop();
	}
}

void mainLoop() {
	//固定帧率处理。
	Framework f = Framework::instance();
	//×按钮被按下了吗？
	if (Framework::instance().isEndRequested()) {
		if (gState) {
			delete gState;
			gState = 0;
		}
		return;
	}
	//初始化第一帧
	if (!gState) {
		File file("1.txt");
		if (!(file.getData())) { //没有数据！
			cout << "stage file could not be read." << endl;
			return;
		}
		gState = new State(file.getData(), file.getSize());
	}

	//获取输入
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
	//结束判断
	if (f.isKeyOn('q')) {
		delete gState;
		gState = 0;
		f.requestEnd();
		return;
	}

	gState->update(dx, dy);

	gState->draw();
	



}