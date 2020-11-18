#pragma once
#ifndef INCLUDED_FILE_H
#define INCLUDED_FILE_H

class File {
public:
	File(const char* fileName);
	~File();
	int getSize() const;
	const char* getData() const;
	//½«char ×ª»»³Éunsigned int
	unsigned getUnsigned(int position) const;
private:
	int mSize;
	char* mData;
};

#endif 