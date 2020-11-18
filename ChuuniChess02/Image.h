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
	//dds格式下图片的尺寸
	int mWidth;
	int mHeight;
	unsigned* mData;//使用1维数组存储2维图片unsigned int信息（32 bit）
};

#endif