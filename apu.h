#ifndef APU_H
#define APU_H

#include <iostream>

class APU{
	public:
		APU();
		unsigned char getReg(unsigned char address);
		void setReg(unsigned char address, unsigned char value);
		void execute();
	private:
		unsigned char* pulse1;
		unsigned char* pulse2;
		unsigned char* triangle;
		unsigned char* noise;
		unsigned char* delta;
		unsigned char status;
		unsigned char frameCounter;

		bool enablePulse1;
		bool enablePulse2;
		bool enableTriangle;
		bool enableNoise;
		bool enableDelta;

		unsigned char pulse1Counter;
		unsigned char pulse2Counter;
		unsigned char triangleCounter;
		unsigned char noiseCounter;
};

#endif