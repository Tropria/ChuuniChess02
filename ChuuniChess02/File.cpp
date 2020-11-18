#include "File.h"
#include <fstream>

using namespace std;
File::File(const char* fileName) : mSize(0), mData(0) {
	ifstream in(fileName, ifstream::binary);
	if (in) {
		in.seekg(0, ifstream::end);
		mSize = static_cast<int>(in.tellg());
		in.seekg(0, ifstream::beg);
		mData = new char[mSize];
		in.read(mData, mSize);
	}
}

File::~File() {
	delete[] mData;
	mData = 0;
}

int File::getSize() const {
	return mSize;
}

const char* File::getData() const {
	return mData;
}

//ȡ��unsigned
unsigned File::getUnsigned(int p) const {
	const unsigned char* up;
	up = reinterpret_cast<const unsigned char*>(mData);
	unsigned r = up[p];
	r |= up[p + 1] << 8;
	r |= up[p + 2] << 16;
	r |= up[p + 3] << 24;
	return r;
}
