#ifndef APU_H
#define APU_H

#include <iostream>

#include "audio.h"

class Audio;

class APU{
	public:
		APU(Audio* audio);
		unsigned char getReg(unsigned char address);
		void setReg(unsigned char address, unsigned char value);
	private:
		Audio* audio;
};

#endif