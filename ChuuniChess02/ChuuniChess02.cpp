#include "GameLib/Framework.h"
using namespace GameLib;

#include "State.h"
#include "File.h"

void mainLoop();

//全局变量
State* gState = 0;
const int gFrameInterval = 8; //16毫秒62.5fps。
unsigned gPreviousTime[10]; //10记录框架时间
int gCounter = 0; //一个计数器，用于计算主循环的次数

#define USE_SLEEP_FUNCTION //使用sleep函数

namespace GameLib {
	void Framework::update() {
		mainLoop();
	}
}

void mainLoop() {
	//固定帧率处理。
	Framework f = Framework::instance();
	while ((f.time() - gPreviousTime[9]) < gFrameInterval) {
#ifdef USE_SLEEP_FUNCTION
		f.sleep(1);
#endif
	}
	//帧率计算
	unsigned currentTime = f.time();
	unsigned frameInterval10 = currentTime - gPreviousTime[0];
	if (gCounter % 60 == 0) { //60每帧显示一次帧率
		cout << "frameInterval:" << frameInterval10 / 10;
		cout << " FrameRate:" << 10 * 1000 / frameInterval10 << endl;
	}
	++gCounter;
	//更新历史
	for (int i = 0; i < 9; ++i) {
		gPreviousTime[i] = gPreviousTime[i + 1];
	}
	gPreviousTime[9] = currentTime;


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
		File file("stageData.txt");
		if (!(file.getData())) { //没有数据！
			cout << "stage file could not be read." << endl;
			return;
		}
		gState = new State(file.getData(), file.getSize());
	}
//cout << "gState.width "<< gState->mWidth<< " height "<< gState->mHeight << endl;
//gState->output();

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

