#ifndef PPU_H
#define PPU_H

#include <iostream>

#include "cpu.h"
#include "colour.h"

constexpr int width = 256;
constexpr int height = 240;

class CPU;

class PPU {
	public:
		PPU(bool mirroring, bool fourScreenMode);
		bool nmi();
		bool stallCPU();
		unsigned char getReg(unsigned short address);
		void setReg(unsigned short address, unsigned char value);
		void execute();
		void oamdma(unsigned char value, CPU* cpu);
		unsigned char* getPixels();
		void dumpPatternTable();
		void dumpNameTable();

	private:
		unsigned char* pixels;
		unsigned char* VRAM;
		unsigned char* SPRRAM;
		unsigned char* secondarySPRRAM;
		unsigned char* currentSprites;
		unsigned char* spriteTiles;
		unsigned char* nameTable0;
		unsigned char* nameTable1;

		unsigned char PPUCTRL;
		unsigned char PPUMASK;
		unsigned char PPUSTATUS;
		unsigned char OAMADDR;
		unsigned char OAMDATA;
		unsigned char PPUSCROLL;
		unsigned char PPUADDR;
		unsigned char PPUDATA;

		unsigned short vramAddr;
		unsigned short tempVramAddr;
		unsigned char fineXScroll;
		unsigned char currentTiles[24];

		bool oddFrame;
		bool nmiOnVblankStart;
		bool slaveSelect;
		bool sixteenMode;
		bool backgroundPatternTableOffset;
		bool spritePatternTableOffset;
		bool vramIncrement32;
		bool blueEmphasis;
		bool greenEmphasis;
		bool redEmphasis;
		bool showSprites;
		bool showBackground;
		bool showSpritesLeft8;
		bool showBackgroundLeft8;
		bool greyscale;
		unsigned char scrollX;
		unsigned char scrollY;
		bool addressLatch;
		bool stallCPUFlag;

		int clockTick;
		int scanline;
		int fineScrollX;
		int fineScrollY;
		unsigned char spriteCount;
		bool hasSpriteZero;

		bool nmiFlag;
		bool singleSceenMirroring;
		bool verticalMirroring;

		void setPixel(int x, int y, unsigned char colourAddress);
		void setPixelTransparent(int x, int y);
		void incXScroll();
		void incYScroll();
		void fetchNextTile();
		void fetchNextSprites();
		void evaluateSprites();
		bool spriteHere(unsigned char x);
		bool spritePriority(unsigned char x);
		bool backgroundTransparent(unsigned char x);
		bool drawSprite(unsigned char x);
		void spriteZero();
		unsigned char nameTableValueAt(unsigned short address);
		void setNameTableValueAt(unsigned short address, unsigned char value);
		unsigned char ppuValueAt(unsigned short address);
		void setPPUValueAt(unsigned short address, unsigned char value);
};

#endif