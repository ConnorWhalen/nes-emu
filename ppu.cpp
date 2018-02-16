#include "ppu.h"

PPU::PPU(bool mirroring, bool fourScreenMode){
	pixels = new unsigned char[width*height*3];
	VRAM = new unsigned char[0x4000];
	SPRRAM = new unsigned char[0x100];
	secondarySPRRAM = new unsigned char[0x20];
	currentSprites = new unsigned char[0x20];
	spriteTiles = new unsigned char[0x10];
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
	for (int i = 0; i < 24; i++) currentTiles[i] = 0;
	spriteCount = 0;
	hasSpriteZero = false;

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
	unsigned char colourValue = ppuValueAt(0x3f00 + colourAddress) & 0x3f;
	if (greyscale) colourValue &= 0x30;
	unsigned char r = Colour::red(colourValue);
	unsigned char g = Colour::green(colourValue);
	unsigned char b = Colour::blue(colourValue);
	Colour::tint(r, g, b, redEmphasis, greenEmphasis, blueEmphasis);
	pixels[y*width*3 + x*3] = r;
	pixels[y*width*3 + x*3 + 1] = g;
	pixels[y*width*3 + x*3 + 2] = b;
}

void PPU::setPixelTransparent(int x, int y){
	if (x >= width || y >= height){
		std::cout<< "Pixel index " << x << ", " << y << " (hex) out of range." << "\n";
	}
	unsigned char colourValue = ppuValueAt(0x3f00) & 0x3f;
	if (greyscale) colourValue &= 0x30;
	unsigned char r = Colour::red(colourValue);
	unsigned char g = Colour::green(colourValue);
	unsigned char b = Colour::blue(colourValue);
	Colour::tint(r, g, b, redEmphasis, greenEmphasis, blueEmphasis);
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
			ret = ppuValueAt(vramAddr);
			// TODO: set PPUDATA to value "under" palette
		} else{
			ret = PPUDATA;
			PPUDATA = ppuValueAt(vramAddr);
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
		vramIncrement32 = (value & 0x04);
		tempVramAddr = (tempVramAddr & 0xf3ff) + ((value & 0x03) << 10);
	} else if (address == 1){
		PPUMASK = value;
		blueEmphasis = (value & 0x80);
		greenEmphasis = (value & 0x40);
		redEmphasis = (value & 0x20);
		showSprites = (value & 0x10);
		showBackground = (value & 0x08);
		showSpritesLeft8 = (value & 0x04);
		showBackgroundLeft8 = (value & 0x02);
		greyscale = (value & 0x01);
	} else if (address == 2){
		PPUSTATUS = (value & 0x1f) + (PPUSTATUS & 0xe0);
	} else if (address == 3){
		OAMADDR = value;
	} else if (address == 4){
		SPRRAM[OAMADDR++] = value;
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
			incXScroll();
			incYScroll();
		}
		if (vramIncrement32) vramAddr += 32; else vramAddr += 1;
	} else{
		std::cout<< "Invalid PPU register address " << int(value) << "\n";
	}
}

void PPU::oamdma(unsigned char value, CPU* cpu){
	for (int i = 0; i < 256; i++){
		SPRRAM[(i+OAMADDR) % 0x100] = cpu->valueAt(value*0x100 + i);
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

void PPU::dumpPatternTable(){
	int x = 0;
	int y = 0;
	int tablePointer = 0;
	for (int i = 0; i < 0x1000; i++){
		for (int j = 0; j < 8; j++){
			unsigned char colour = ((ppuValueAt(tablePointer) >> (7-j)) & 0x01) + ((ppuValueAt(tablePointer + 8) >> (6-j)) & 0x02);
			setPixel(x, y, colour);
			x++;
		}
		x -= 8;
		y++;
		if (++tablePointer % 8 == 0){
			tablePointer += 8;
			x += 8;
			y -= 8;
		}
		if (x == 256){
			x = 0;
			y += 8;
		}
	}
}

void PPU::dumpNameTable(){
	std::cout<< std::hex << "*****FIRST NAMETABLE*****" << "\n";
	for (int i = 0x2000; i < 0x23c0; i++){
		std::cout<< int(ppuValueAt(i)) << "\n";
	}
	std::cout<< std::hex << "*****SECOND NAMETABLE*****" << "\n";
	for (int i = 0x2c00; i < 0x2fc0; i++){
		std::cout<< int(ppuValueAt(i)) << "\n";
	}
}

void PPU::execute(){
	if (scanline < 0){ // prerender
		if (clockTick == 1) PPUSTATUS &= 0x1f; // clear flags
		else if (clockTick == 339 && oddFrame) clockTick++; // Skip last tick
		if (showBackground || showSprites){
			if (clockTick == 256) incYScroll();
			else if (clockTick == 257) vramAddr = (vramAddr & 0x7be0) + (tempVramAddr & 0x041f); // reset x scroll
			else if (clockTick >= 280 && clockTick < 305) vramAddr = (vramAddr & 0x041f) + (tempVramAddr & 0x7be0); // reset y scroll
			else if (clockTick == 321 || clockTick == 329) fetchNextTile();
			else if (clockTick == 328 || clockTick == 336) incXScroll();

			if (clockTick == 320) fetchNextSprites();
			else if (clockTick >= 257 && clockTick < 321) OAMADDR = 0;
		}
	} else if (scanline < 240){ // visible lines
		if (showBackground || showSprites){
			if (clockTick == 0) clockTick = 0;// pass
			else if (clockTick % 8 == 1 && (clockTick < 256 || clockTick == 321 || clockTick == 329)) fetchNextTile();
			else if (clockTick % 8 == 0 && (clockTick < 256 || clockTick > 320)) incXScroll();
			else if (clockTick == 256) incYScroll();
			else if (clockTick == 257) vramAddr = (vramAddr & 0x7be0) + (tempVramAddr & 0x041f); // reset x scroll

			if (clockTick == 64) for (int i = 0; i < 0x20; i++) secondarySPRRAM[i] = 0xff;
			else if (clockTick == 65) evaluateSprites();
			else if (clockTick == 320) fetchNextSprites();
			else if (clockTick >= 257 && clockTick < 321) OAMADDR = 0;

			// Draw Pixel
			if (clockTick < 9 && clockTick > 0){
				if (showBackground && showBackgroundLeft8 && showSprites && showSpritesLeft8){
					if (spriteHere(clockTick-1) && (!spritePriority(clockTick-1) || backgroundTransparent(clockTick-1))){
						if (!drawSprite(clockTick-1)){
							setPixel(clockTick-1, scanline, currentTiles[((clockTick-1) % 8) + fineXScroll]);
						}
					} else{
						setPixel(clockTick-1, scanline, currentTiles[((clockTick-1) % 8) + fineXScroll]);
					}
					spriteZero();
				} else if ((showBackground && showBackgroundLeft8) && !(showSprites && showSpritesLeft8)){
					setPixel(clockTick-1, scanline, currentTiles[((clockTick-1) % 8) + fineXScroll]);
				} else if (!(showBackground && showBackgroundLeft8) && (showSprites && showSpritesLeft8)){
					if (spriteHere(clockTick-1)){
						if (!drawSprite(clockTick-1)){
							setPixelTransparent(clockTick-1, scanline);
						}
					} else{
						setPixelTransparent(clockTick-1, scanline);
					}
				} else{ // !(showback && showback8) && !(showspr && showspr8)
					setPixelTransparent(clockTick-1, scanline);
				}
			} else if (clockTick < 257 && clockTick > 0){
				if (showBackground && showSprites){
					if (spriteHere(clockTick-1) && (!spritePriority(clockTick-1) || backgroundTransparent(clockTick-1))){
						if (!drawSprite(clockTick-1)){
							setPixel(clockTick-1, scanline, currentTiles[((clockTick-1) % 8) + fineXScroll]);
						}
					} else{
						setPixel(clockTick-1, scanline, currentTiles[((clockTick-1) % 8) + fineXScroll]);
					}
					if (clockTick < 256) spriteZero();
				} else if (showBackground && !showSprites){
					setPixel(clockTick-1, scanline, currentTiles[((clockTick-1) % 8) + fineXScroll]);
				} else if (!showBackground && showSprites){
					if (spriteHere(clockTick-1)){
						if (!drawSprite(clockTick-1)){
							setPixelTransparent(clockTick-1, scanline);
						}
					} else{
						setPixelTransparent(clockTick-1, scanline);
					}
				} else{ // !showBackground && !showSprites
					setPixelTransparent(clockTick-1, scanline);
				}
			}
		}
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
			// for (int i = 0; i < 16; i++) std::cout << int(PPU::ppuValueAt(0x3f00 + i)) << " ";
			// std::cout << "\n";
		}
	}
}

void PPU::fetchNextTile(){
	for (int i = 0; i < 16; i++) currentTiles[i] = currentTiles[i + 8];
	unsigned char nameTableFetch = ppuValueAt((vramAddr & 0x0fff) + 0x2000);
	unsigned char attributeTableFetch = ppuValueAt((vramAddr & 0x0c00) + ((vramAddr & 0x0380) >> 4) + ((vramAddr & 0x001c) >> 2) + 0x23c0);
	unsigned short patternTableAddress = ((vramAddr & 0x7000) >> 12) + (nameTableFetch << 4);
	if (backgroundPatternTableOffset) patternTableAddress += 0x1000;
	unsigned char patternTableLoFetch = ppuValueAt(patternTableAddress);
	unsigned char patternTableHiFetch = ppuValueAt(patternTableAddress + 0x0008);
	for (int i = 0; i < 7; i++) currentTiles[i + 16] = ((patternTableLoFetch >> (7-i)) & 0x1) + ((patternTableHiFetch >> (6-i)) & 0x2);
	// shifting by a negative number results in undefined behaviour
	currentTiles[23] = (patternTableLoFetch & 0x1) + ((patternTableHiFetch << 1) & 0x2);
	unsigned char attributeBitShift = 0;
	if ((vramAddr & 0x0002) == 0x0002) attributeBitShift += 2;
	if ((vramAddr & 0x0040) == 0x0040) attributeBitShift += 4;
	for (int i = 0; i < 8; i++) currentTiles[i + 16] += ((attributeTableFetch >> attributeBitShift) & 0x03) << 2;
	// std::cout<< std::hex << patternTableAddress << " " << int(vramAddr) << " " << int(nameTableFetch) << "\n";
}

void PPU::incXScroll(){
	if ((vramAddr & 0x001f) == 31){
		vramAddr -= 31;
		vramAddr ^= 0x0400;
	}
	else vramAddr += 1;
}

void PPU::incYScroll(){
	if ((vramAddr & 0x7000) != 0x7000) vramAddr += 0x1000;
	else{
		vramAddr -= 0x7000;
		unsigned char y = (vramAddr & 0x03e0) >> 5;
		if (y == 29){
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

void PPU::fetchNextSprites(){
	for (int i = 0; i < 0x20; i++) currentSprites[i] = secondarySPRRAM[i];
	if (!sixteenMode){
		for (int i = 0; i < spriteCount; i++){
			unsigned char yOffset = scanline - currentSprites[i*4];
			if ((currentSprites[i*4 + 2] & 0x80) == 0x80) yOffset = 0x07 - yOffset; // V flip
			if (yOffset > 7) yOffset += 8;
			unsigned short patternTableAddress = currentSprites[i*4 + 1] << 4;
			if (spritePatternTableOffset) patternTableAddress += 0x1000;
			spriteTiles[i*2] = ppuValueAt(patternTableAddress + yOffset);
			spriteTiles[i*2 + 1] = ppuValueAt(patternTableAddress + yOffset + 8);
			if ((currentSprites[i*4 + 2] & 0x40) == 0x40){ // H flip
				spriteTiles[i*2] = (spriteTiles[i*2] & 0xF0) >> 4 | (spriteTiles[i*2] & 0x0F) << 4;
				spriteTiles[i*2] = (spriteTiles[i*2] & 0xCC) >> 2 | (spriteTiles[i*2] & 0x33) << 2;
				spriteTiles[i*2] = (spriteTiles[i*2] & 0xAA) >> 1 | (spriteTiles[i*2] & 0x55) << 1;
				spriteTiles[i*2 + 1] = (spriteTiles[i*2 + 1] & 0xF0) >> 4 | (spriteTiles[i*2 + 1] & 0x0F) << 4;
				spriteTiles[i*2 + 1] = (spriteTiles[i*2 + 1] & 0xCC) >> 2 | (spriteTiles[i*2 + 1] & 0x33) << 2;
				spriteTiles[i*2 + 1] = (spriteTiles[i*2 + 1] & 0xAA) >> 1 | (spriteTiles[i*2 + 1] & 0x55) << 1;
			}
		}
	} else{
		for (int i = 0; i < spriteCount; i++){
			unsigned char yOffset = scanline - currentSprites[i*4];
			if ((currentSprites[i*4 + 2] & 0x80) == 0x80) yOffset = 0x0f - yOffset; // V flip
			unsigned short patternTableAddress = (currentSprites[i*4 + 1] & 0xfe) << 4;
			if ((currentSprites[i*4 + 1] & 0x01) == 0x01) patternTableAddress += 0x1000;
			spriteTiles[i*2] = ppuValueAt(patternTableAddress + yOffset);
			spriteTiles[i*2 + 1] = ppuValueAt(patternTableAddress + yOffset + 8);
			if ((currentSprites[i*4 + 2] & 0x40) == 0x40){ // H flip
				spriteTiles[i*2] = (spriteTiles[i*2] & 0xF0) >> 4 | (spriteTiles[i*2] & 0x0F) << 4;
				spriteTiles[i*2] = (spriteTiles[i*2] & 0xCC) >> 2 | (spriteTiles[i*2] & 0x33) << 2;
				spriteTiles[i*2] = (spriteTiles[i*2] & 0xAA) >> 1 | (spriteTiles[i*2] & 0x55) << 1;
				spriteTiles[i*2 + 1] = (spriteTiles[i*2 + 1] & 0xF0) >> 4 | (spriteTiles[i*2 + 1] & 0x0F) << 4;
				spriteTiles[i*2 + 1] = (spriteTiles[i*2 + 1] & 0xCC) >> 2 | (spriteTiles[i*2 + 1] & 0x33) << 2;
				spriteTiles[i*2 + 1] = (spriteTiles[i*2 + 1] & 0xAA) >> 1 | (spriteTiles[i*2 + 1] & 0x55) << 1;
			}
		}
	}
}

void PPU::evaluateSprites(){
	spriteCount = 0;
	hasSpriteZero = false;
	unsigned short primaryIndex = OAMADDR; // 0-0xff array index since it can be misaligned
	unsigned char secondaryIndex = 0; // 0-7 sprite number
	unsigned char subIndex = 0;
	unsigned char height = sixteenMode ? 16 : 8;
	while (primaryIndex <= 0xfc && secondaryIndex <= 8){
		secondarySPRRAM[secondaryIndex*4] = SPRRAM[primaryIndex];
		if (secondarySPRRAM[secondaryIndex*4] <= scanline && secondarySPRRAM[secondaryIndex*4] > scanline - height){
			if (primaryIndex == 0) hasSpriteZero = true;
			secondarySPRRAM[secondaryIndex*4 + 1] = SPRRAM[primaryIndex + 1];
			secondarySPRRAM[secondaryIndex*4 + 2] = SPRRAM[primaryIndex + 2];
			secondarySPRRAM[secondaryIndex*4 + 3] = SPRRAM[primaryIndex + 3];
			secondaryIndex++;
			spriteCount++;
		}
		primaryIndex += 4;
	}
	if (secondaryIndex == 8){
		while (primaryIndex <= 0xfc){
			if (SPRRAM[primaryIndex + subIndex] <= scanline && SPRRAM[primaryIndex + subIndex] > scanline - height){
				PPUCTRL |= 0x20; // sprite overflow
				break;
			}
			primaryIndex++;
			subIndex = (subIndex + 1) % 4;
		}
	}
}

bool PPU::spriteHere(unsigned char x){
	for (int i = 0; i < spriteCount; i++){
		if (currentSprites[i*4 + 3] <= x && currentSprites[i*4 + 3] > x - 8){
			return true;
		}
	}
	return false;
}

bool PPU::spritePriority(unsigned char x){
	for (int i = 0; i < spriteCount; i++){
		if (currentSprites[i*4 + 3] <= x && currentSprites[i*4 + 3] > x - 8){
			if ((currentSprites[i*4 + 2] & 0x20) == 0x20) return true;
			else return false;
		}
	}
	return false;
}

bool PPU::backgroundTransparent(unsigned char x){
	return (currentTiles[((clockTick-1) % 8) + fineXScroll] % 4 == 0);
}

bool PPU::drawSprite(unsigned char x){
	unsigned char tile0 = 0;
	unsigned char tile1 = 0;
	unsigned char attribute = 0;
	unsigned char xOffset = 0;
	for (int i = 0; i < spriteCount; i++){
		if (currentSprites[i*4 + 3] <= x && currentSprites[i*4 + 3] > x - 8){
			tile0 = spriteTiles[i*2];
			tile1 = spriteTiles[i*2 + 1];
			attribute = currentSprites[i*4 + 2] & 0x03;
			xOffset = x - currentSprites[i*4 + 3];

			unsigned char colourAddress = (tile0 >> (7 - xOffset)) & 0x01;
			if (xOffset < 7) colourAddress += (tile1 >> (6 - xOffset)) & 0x02;
			else colourAddress += (tile1 << 1) & 0x02; // shifting by a negative number results in undefined behaviour
			colourAddress += attribute << 2;
			if (colourAddress % 4 != 0){
				setPixel(x, scanline, colourAddress | 0x10);
				return true;
			}
		}
	}
	return false;
}

void PPU::spriteZero(){
	if (hasSpriteZero && currentSprites[3] <= clockTick-1 && currentSprites[3] > clockTick-9){
		unsigned char xOffset = clockTick-1 - currentSprites[3];
		unsigned char colourAddress = (spriteTiles[0] >> (7 - xOffset)) & 0x01;
		if (xOffset < 7) colourAddress += (spriteTiles[1] >> (6 - xOffset)) & 0x02;
		else colourAddress += (spriteTiles[1] << 1) & 0x02; // shifting by a negative number results in undefined behaviour

		if (!backgroundTransparent(clockTick-1) && colourAddress != 0) PPUSTATUS |= 0x40;
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
	// std::cout << std::hex << "Nametable Write: " << int(value) << " " << int(address) << "\n";
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
		ret = nameTableValueAt(address);
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
		setNameTableValueAt(address, value);
	} else{
		VRAM[address] = value;
	}
}