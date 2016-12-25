#include "ppu.h"

void PPU::setPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b){
	if (x >= width || y >= height){
		std::cout<< "Pixel index " << x << ", " << y << " (hex) out of range." << "\n";
	}
	pixels[y*width*3 + x*3] = r;
	pixels[y*width*3 + x*3 + 1] = g;
	pixels[y*width*3 + x*3 + 2] = b;
}

unsigned char PPU::getReg(unsigned short address){
	return 0;
}
void PPU::setReg(unsigned short address, unsigned char value){

}