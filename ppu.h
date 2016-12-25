#ifndef PPU_H
#define PPU_H

#include <iostream>

const int width = 256;
const int height = 240;
extern unsigned char pixels[width*height*3];

namespace PPU{
	void setPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b);
	unsigned char getReg(unsigned short address);
	void setReg(unsigned short address, unsigned char value);
}

#endif