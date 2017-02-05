#include "ppu.h"

PPU::PPU(bool mirroring, bool fourScreenMode){
	pixels = new unsigned char[width*height*3];
	VRAM = new unsigned char[0x4000];
	SPRRAM = new unsigned char[0x100];
	secondarySPRRAM = new unsigned char[0x20];
	nameTable0 = new unsigned char[0x400];
	nameTable1 = new unsigned char[0x400];

	PPUCTRL = 0;
	PPUMASK = 0;
	PPUSTATUS = 0;
	OAMADDR = 0;
	OAMDATA = 0;
	PPUSCROLL = 0;
	PPUADDR = 0;
	PPUDATA = 0;

	vramAddr = 0;
	tempVramAddr = 0;
	fineXScroll = 0;
	for (int i = 0; i < 16; i++) currentTiles[i] = 0;

	oddFrame = false;
	nmiOnVblankStart = false;
	sixteenMode = false;
	backgroundPatternTableOffset = false;
	spritePatternTableOffset = false;
	vramIncrement32 = false;
	blueEmphasis = false;
	greenEmphasis = false;
	redEmphasis = false;
	showSprites = false;
	showBackground = false;
	showSpritesLeft8 = false;
	showBackgroundLeft8 = false;
	greyscale = false;
	addressLatch = false;
	stallCPUFlag = false;

	clockTick = 0;
	scanline = -1;

	nmiFlag = false;

	if (fourScreenMode){
		singleSceenMirroring = true;
		verticalMirroring = false;
	} else{
		singleSceenMirroring = false;
		if (mirroring){
			verticalMirroring = true;
		} else{
			verticalMirroring = false;
		}
	}
}

unsigned char* PPU::getPixels(){
	return pixels;
}

void PPU::setPixel(int x, int y, unsigned char colourAddress){
	if (x >= width || y >= height){
		std::cout<< "Pixel index " << x << ", " << y << " (hex) out of range." << "\n";
	}
	unsigned char colourValue = PPU::ppuValueAt(0x3f00 & colourAddress);
	unsigned char r = Colour::red(colourValue);
	unsigned char g = Colour::green(colourValue);
	unsigned char b = Colour::blue(colourValue);
	pixels[y*width*3 + x*3] = r;
	pixels[y*width*3 + x*3 + 1] = g;
	pixels[y*width*3 + x*3 + 2] = b;
}

bool PPU::nmi(){
	if ((PPUSTATUS & 0x80) == 0x80 && nmiOnVblankStart && !nmiFlag){
		nmiFlag = true;
		return true;
	} else if((PPUSTATUS & 0x80) == 0x80 && nmiOnVblankStart){ // nmiFlag still true
		return false;
	} else{
		nmiFlag = false;
		return false;
	}
}

unsigned char PPU::getReg(unsigned short address){
	unsigned char ret = 0;
	if (address == 0){
		ret = PPUCTRL;
	} else if (address == 1){
		ret = PPUMASK;
	} else if (address == 2){
		ret = PPUSTATUS;
		PPUSTATUS &= 0x7f;
		addressLatch = false;
	} else if (address == 3){
		ret = OAMADDR;
	} else if (address == 4){
		ret = SPRRAM[OAMADDR];
	} else if (address == 5){
		ret = PPUSCROLL;
	} else if (address == 6){
		ret = PPUADDR;
	} else if (address == 7){
		if ((vramAddr % 0x4000) > 0x3eff){
			ret = PPU::ppuValueAt(vramAddr);
			// TODO: set PPUDATA to value "under" palette
		} else{
			ret = PPUDATA;
			PPUDATA = PPU::ppuValueAt(vramAddr);
		}
		if (vramIncrement32) vramAddr += 32; else vramAddr += 1;
	} else{
		std::cout<< "Invalid PPU register address " << int(address) << "\n";
	}
	return ret;
}

void PPU::setReg(unsigned short address, unsigned char value){
	if (address == 0){
		PPUCTRL = value;
		nmiOnVblankStart = (value & 0x80);
		slaveSelect = (value & 0x40);
		sixteenMode = (value & 0x20);
		backgroundPatternTableOffset = (value & 0x10);
		spritePatternTableOffset = (value & 0x08);
		vramIncrement32 = (value & 0x40);
		tempVramAddr = (tempVramAddr & 0xf3ff) + ((value & 0x03) << 10);
	} else if (address == 1){
		PPUMASK = value;
		blueEmphasis = (value & 0x80);
		greenEmphasis = (value & 0x40);
		redEmphasis = (value & 0x20);
		showSprites = (value & 0x10);
		showBackground = (value & 0x08);
		showSpritesLeft8 = (value & 0x40);
		showBackgroundLeft8 = (value & 0x20);
		greyscale = (value & 0x01);
	} else if (address == 2){
		PPUSTATUS = (value & 0x1f) + (PPUSTATUS & 0xe0);
	} else if (address == 3){
		OAMADDR = value;
	} else if (address == 4){
		SPRRAM[OAMDATA++] = value;
	} else if (address == 5){
		PPUSCROLL = value;
		if (addressLatch){
			tempVramAddr = (tempVramAddr & 0x0c1f) + ((value & 0xf8) << 2) + ((value & 0x07) << 12);
		} else{
			tempVramAddr = (tempVramAddr & 0xffe0) + ((value & 0xf8) >> 3);
			fineXScroll = value & 0x07;
		}
		addressLatch = !addressLatch;
	} else if (address == 6){
		PPUADDR = value;
		if (addressLatch){
			tempVramAddr = (tempVramAddr & 0xff00) + value;
			vramAddr = tempVramAddr;
		} else{
			tempVramAddr = (tempVramAddr & 0x00ff) + ((value & 0x3f) << 8);
		}
		addressLatch = !addressLatch;
	} else if (address == 7){
		setPPUValueAt(vramAddr, value);
		if ((scanline > -2 && scanline < 240) && (showBackground || showSprites)){
			PPU::incXScroll();
			PPU::incYScroll();
		}
		if (vramIncrement32) vramAddr += 32; else vramAddr += 1;
	} else{
		std::cout<< "Invalid PPU register address " << int(value) << "\n";
	}
}

void PPU::oamdma(unsigned char value, CPU* cpu){
	for (int i = 0; i < 256; i++){
		SPRRAM[(i+OAMADDR) % 0x100] = cpu->valueAt(value*0x100 + ((i+OAMADDR) % 0x100));
	}
	stallCPUFlag = true;
}

bool PPU::stallCPU(){
	if (stallCPUFlag){
		stallCPUFlag = false;
		return true;
	} else{
		return false;
	}
}

void PPU::execute(){
	if (scanline < 0){ // prerender
		if (clockTick == 1) PPUSTATUS &= 0x1f; // clear flags
		else if (clockTick == 256) PPU::incYScroll();
		else if (clockTick == 257) vramAddr = (vramAddr & 0x7be0) + (tempVramAddr & 0x041f); // reset x scroll
		else if (clockTick <= 280 && clockTick < 305) vramAddr = (vramAddr & 0x041f) + (tempVramAddr & 0x7be0); // reset y scroll
		else if (clockTick == 321 || clockTick == 329) fetchNextTile();
		else if (clockTick == 328 || clockTick == 336) PPU::incXScroll();
		else if (clockTick == 339 && oddFrame) clockTick++; // Skip last tick

	} else if (scanline < 240){ // visible lines
		if (clockTick == 0) clockTick = 0;// pass
		else if (clockTick % 8 == 1 && clockTick < 256 && clockTick > 320) fetchNextTile();
		else if (clockTick % 8 == 0 && clockTick < 256 && clockTick > 320) PPU::incXScroll();
		else if (clockTick == 256) PPU::incYScroll();
		else if (clockTick == 257) vramAddr = (vramAddr & 0x7be0) + (tempVramAddr & 0x041f); // reset x scroll

		if (clockTick < 257) PPU::setPixel(clockTick-1, scanline, currentTiles[((clockTick-1) % 8) + fineXScroll]);
	} else if (scanline < 241){ // postrender
		// pass
	} else if (scanline < 261){ // vblank
		if (scanline == 241 && clockTick == 1){
			PPUSTATUS |= 0x80; // set vblank flag
		}
	}

	if (++clockTick == 341){
		clockTick = 0;
		if (++scanline == 261){
			scanline = -1;
			oddFrame = !oddFrame;
		}
	}
}

void PPU::fetchNextTile(){
	for (int i = 0; i < 8; i++) currentTiles[i] = currentTiles[i + 8];
	unsigned char nameTableFetch = PPU::ppuValueAt((vramAddr & 0x0fff) + 0x2000);
	unsigned char attributeTableFetch = PPU::ppuValueAt((vramAddr & 0x0c00) + ((vramAddr & 0x0380) >> 4) + ((vramAddr & 0x001c) >> 2) + 0x23c0);
	unsigned short patternTableAddress = ((vramAddr & 0x7000) >> 12) + (nameTableFetch << 8);
	if (backgroundPatternTableOffset) patternTableAddress += 0x1000;
	unsigned char patternTableLoFetch = PPU::ppuValueAt(patternTableAddress);
	unsigned char patternTableHiFetch = PPU::ppuValueAt(patternTableAddress + 0x0008);
	for (int i = 0; i < 8; i++) currentTiles[i + 8] = ((patternTableLoFetch >> i) & 0x1) + ((patternTableHiFetch >> (i-1)) & 0x2);
	unsigned char attributeBitShift = 0;
	if ((vramAddr & 0x0002) == 0x0002) attributeBitShift += 2;
	if ((vramAddr & 0x0040) == 0x0040) attributeBitShift += 4;
	for (int i = 0; i < 8; i++) currentTiles[i + 8] += ((attributeTableFetch >> attributeBitShift) & 0x03) << 2;
}

void PPU::incXScroll(){
	if ((vramAddr & 0x001f) == 31){
		vramAddr -= 31;
		vramAddr ^= 0x0400;
	}
	else vramAddr += 1;
}

void PPU::incYScroll(){
	if ((vramAddr & 0x7000) == 0x7000) vramAddr += 0x1000;
	else{
		vramAddr -= 0x7000;
		unsigned char y = (vramAddr & 0x03e0) >> 5;
		if (y == 30){
			y = 0;
			vramAddr ^= 0x0800;
		} else if (y == 31){
			y = 0;
		} else {
			y += 1;
		}
		vramAddr = (vramAddr & 0xfc1f) + (y << 5);
	}
}

unsigned char PPU::nameTableValueAt(unsigned short address){
	unsigned char tableNumber = 0;
	if (address > 0x2fff){
		std::cout<< "Invalid Nametable address " << address << "\n";
	} else if (address > 0x2bff){ // logical NT3
		if (singleSceenMirroring) tableNumber = 0;
		else if (verticalMirroring) tableNumber = 1;
		else tableNumber = 1;
	} else if (address > 0x27ff){ // logical NT2
		if (singleSceenMirroring) tableNumber = 0;
		else if (verticalMirroring) tableNumber = 0;
		else tableNumber = 1;
	} else if (address > 0x23ff){ // logical NT1
		if (singleSceenMirroring) tableNumber = 0;
		else if (verticalMirroring) tableNumber = 1;
		else tableNumber = 0;
	} else if (address > 0x1fff){ // logical NT0
		if (singleSceenMirroring) tableNumber = 0;
		else if (verticalMirroring) tableNumber = 0;
		else tableNumber = 0;
	} else{
		std::cout<< "Invalid Nametable address " << address << "\n";
	}

	if (tableNumber == 0){
		return nameTable0[address % 0x400];
	} else if (tableNumber == 1){
		return nameTable1[address % 0x400];
	}
	return 0;
}

void PPU::setNameTableValueAt(unsigned short address, unsigned char value){
	unsigned char tableNumber = 0;
	if (address > 0x2fff){
		std::cout<< "Invalid Nametable address " << address << "\n";
	} else if (address > 0x2bff){ // logical NT3
		if (singleSceenMirroring) tableNumber = 0;
		else if (verticalMirroring) tableNumber = 1;
		else tableNumber = 1;
	} else if (address > 0x27ff){ // logical NT2
		if (singleSceenMirroring) tableNumber = 0;
		else if (verticalMirroring) tableNumber = 0;
		else tableNumber = 1;
	} else if (address > 0x23ff){ // logical NT1
		if (singleSceenMirroring) tableNumber = 0;
		else if (verticalMirroring) tableNumber = 1;
		else tableNumber = 0;
	} else if (address > 0x1fff){ // logical NT0
		if (singleSceenMirroring) tableNumber = 0;
		else if (verticalMirroring) tableNumber = 0;
		else tableNumber = 0;
	} else{
		std::cout<< "Invalid Nametable address " << address << "\n";
	}

	if (tableNumber == 0){
		nameTable0[address % 0x400] = value;
	} else if (tableNumber == 1){
		nameTable1[address % 0x400] = value;
	}
}

unsigned char PPU::ppuValueAt(unsigned short address){
	address = address % 0x4000;
	unsigned char ret;
	if (address > 0x3eff){
		if (address % 4 == 0){
			ret = VRAM[0x3f00];
		} else{
			ret = VRAM[((address-0x3f00) % 0x20) + 0x3f00];
		}
	} else if (address > 0x1fff){
		ret = VRAM[((address-0x2000) % 0x1000) + 0x2000];
	} else{
		ret = VRAM[address];
	}
	return ret;
}

void PPU::setPPUValueAt(unsigned short address, unsigned char value){
	address = address % 0x4000;
	if (address > 0x3eff){
		if (address % 4 == 0){
			VRAM[0x3f00] = value;
		} else{
			VRAM[((address-0x3f00) % 0x20) + 0x3f00] = value;
		}
	} else if (address > 0x1fff){
		VRAM[((address-0x2000) % 0x1000) + 0x2000] = value;
	} else{
		VRAM[address] = value;
	}
}