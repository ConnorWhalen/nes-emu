#ifndef NINTENDULATOR_H
#define NINTENDULATOR_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <thread>

#include "execute.h"
#include "ppu.h"
#ifdef MAC
	#include "view.h"
#endif
#ifdef PI
	#include "piView.h"
#endif

extern bool debug;

extern unsigned short programRomPageCount;
extern unsigned short characterRomPageCount;
extern unsigned char flags1;
extern unsigned char flags2;
extern unsigned short mapper;
extern unsigned char submapperNumber;
extern bool fourScreenMode;
extern bool trainer;
extern bool batteryBackedRam;
extern bool mirroring;
extern bool nes2Mode;
extern bool playchoice10;
extern bool vsUnisystem;

extern unsigned short PC;
extern unsigned char SP;
extern unsigned char A;
extern unsigned char X;
extern unsigned char Y;
extern unsigned char P;

extern std::vector<unsigned char> romBytes;
extern unsigned char RAM[0x0800];
extern unsigned char cartRAM[0x2000];
extern unsigned char programBank0;
extern unsigned char programBank1;

const int periodMicroseconds = 1000000 / 60;

const unsigned char carryMask = 0x01;
const unsigned char zeroMask = 0x02;
const unsigned char interruptDisableMask = 0x04;
const unsigned char decimalMask = 0x08;
const unsigned char breakMask = 0x10;
const unsigned char overflowMask = 0x40;
const unsigned char negativeMask = 0x80;

unsigned char valueAt(unsigned short address);
void setValueAt(unsigned short address, unsigned char value);

#endif