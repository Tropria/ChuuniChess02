#pragma once
#ifndef INCLUDED_IMAGE_H
#define INCLUDED_IMAGE_H

class Image {
public:
	Image(const char* fileName);
	~Image();
	int getWidth() const;
	int getHeight() const;
	void draw(
		int dstX,
		int dstY,
		int srcX,
		int srcY,
		int width,
		int height) const;
private:
	//dds��ʽ��ͼƬ�ĳߴ�
	int mWidth;
	int mHeight;
	unsigned* mData;//ʹ��1ά����洢2άͼƬunsigned int��Ϣ��32 bit��
};

#endif