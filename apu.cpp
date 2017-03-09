#include "apu.h"

APU::APU(){
	pulse1 = new unsigned char[4];
	pulse2 = new unsigned char[4];
	triangle = new unsigned char[4];
	noise = new unsigned char[4];
	delta = new unsigned char[4];
	status = 0x00;
	frameCounter = 0x00;

	enablePulse1 = false;
	enablePulse2 = false;
	enableTriangle = false;
	enableNoise = false;
	enableDelta = false;

	pulse1Counter = 0x00;
	pulse2Counter = 0x00;
	triangleCounter = 0x00;
	noiseCounter = 0x00;
}

void APU::execute(){

}

unsigned char APU::getReg(unsigned char address){
	unsigned char ret = 0;
	switch (address){
		case 0: ret = pulse1[0];
				break;
		case 1: ret = pulse1[1];
				break;
		case 2: ret = pulse1[2];
				break;
		case 3: ret = pulse1[3];
				break;
		case 4: ret = pulse2[0];
				break;
		case 5: ret = pulse2[1];
				break;
		case 6: ret = pulse2[2];
				break;
		case 7: ret = pulse2[3];
				break;
		case 8: ret = triangle[0];
				break;
		case 9: ret = triangle[1];
				break;
		case 10: ret = triangle[2];
				break;
		case 11: ret = triangle[3];
				break;
		case 12: ret = noise[0];
				break;
		case 13: ret = noise[1];
				break;
		case 14: ret = noise[2];
				break;
		case 15: ret = noise[3];
				break;
		case 16: ret = delta[0];
				break;
		case 17: ret = delta[1];
				break;
		case 18: ret = delta[2];
				break;
		case 19: ret = delta[3];
				break;
		case 22: ret = status;
				break;
		case 23: ret = frameCounter;
				break;
		default: std::cout << "Invaled APU Address " << std::hex << int(address) << "\n";
				break;
	}
	return ret;
}

void APU::setReg(unsigned char address, unsigned char value){
	switch (address){
		case 0: pulse1[0] = value;
				break;
		case 1: pulse1[1] = value;
				break;
		case 2: pulse1[2] = value;
				break;
		case 3: pulse1[3] = value;
				break;
		case 4: pulse2[0] = value;
				break;
		case 5: pulse2[1] = value;
				break;
		case 6: pulse2[2] = value;
				break;
		case 7: pulse2[3] = value;
				break;
		case 8: triangle[0] = value;
				break;
		case 9: triangle[1] = value;
				break;
		case 10: triangle[2] = value;
				break;
		case 11: triangle[3] = value;
				break;
		case 12: noise[0] = value;
				break;
		case 13: noise[1] = value;
				break;
		case 14: noise[2] = value;
				break;
		case 15: noise[3] = value;
				break;
		case 16: delta[0] = value;
				break;
		case 17: delta[1] = value;
				break;
		case 18: delta[2] = value;
				break;
		case 19: delta[3] = value;
				break;
		case 21: enableDelta = ((value & 0x10) == 0x10);
				 enableNoise = ((value & 0x08) == 0x08);
				 enableTriangle = ((value & 0x04) == 0x04);
				 enablePulse2 = ((value & 0x02) == 0x02);
				 enablePulse1 = ((value & 0x01) == 0x01);
				 if (!enableNoise) noiseCounter = 0;
				 if (!enableTriangle) triangleCounter = 0;
				 if (!enablePulse2) pulse2Counter = 0;
				 if (!enablePulse1) pulse1Counter = 0;
				break;
		case 23: frameCounter = value;
				break;
		default: std::cout << "Invaled APU Address " << std::hex << int(address) << "\n";
				break;
	}
}