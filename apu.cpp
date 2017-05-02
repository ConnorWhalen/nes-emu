#include "apu.h"

APU::APU(Audio* audio){
	this->audio = audio;
}

unsigned char APU::getReg(unsigned char address){
	unsigned char ret = 0;
	switch (address){
		case 0x15: ret = audio->getStatus();
				   break;
		default: std::cout << "Invaled APU Address " << std::hex << int(address) << "\n";
				   break;
	}
	return ret;
}

void APU::setReg(unsigned char address, unsigned char value){
	switch (address){
		case 0x00: audio->setPulse1Duty(value >> 6);
				   audio->setPulse1Halt((value & 0x20) == 0x20);
				   audio->setPulse1Envelope((value & 0x10) == 0x10);
				   audio->setPulse1EnvelopePeriod(value & 0x0f);
				   break;
		case 0x01: audio->setPulse1Sweep((value & 0x80) == 0x80);
				   audio->setPulse1SweepPeriod((value & 0x70) >> 4);
				   audio->setPulse1SweepNegative((value & 0x08) == 0x08);
				   audio->setPulse1SweepShift(value & 0x07);
				   break;
		case 0x02: audio->setPulse1TimerLow(value);
				   break;
		case 0x03: audio->setPulse1Length(value >> 3);
				   audio->setPulse1TimerHigh(value & 0x07);
				   break;
		case 0x04: audio->setPulse2Duty(value >> 6);
				   audio->setPulse2Halt((value & 0x20) == 0x20);
				   audio->setPulse2Envelope((value & 0x10) == 0x10);
				   audio->setPulse2EnvelopePeriod(value & 0x0f);
				   break;
		case 0x05: audio->setPulse2Sweep((value & 0x80) == 0x80);
				   audio->setPulse2SweepPeriod((value & 0x70) >> 4);
				   audio->setPulse2SweepNegative((value & 0x08) == 0x08);
				   audio->setPulse2SweepShift(value & 0x07);
				   break;
		case 0x06: audio->setPulse2TimerLow(value);
				   break;
		case 0x07: audio->setPulse2Length(value >> 3);
				   audio->setPulse2TimerHigh(value & 0x07);
				   break;
		case 0x08: audio->setTriangleHalt((value & 0x80) == 0x80);
				   audio->setTriangleFrameCount(value & 0x7f);
				   break;
		case 0x09: // pass
				   break;
		case 0x0a: audio->setTriangleTimerLow(value);
				   break;
		case 0x0b: audio->setTriangleLength(value >> 3);
				   audio->setTriangleTimerHigh(value & 0x07);
				   break;
		case 0x0c: audio->setNoiseHalt((value & 0x20) == 0x20);
				   audio->setNoiseEnvelope((value & 0x10) == 0x10);
				   audio->setNoiseEnvelopePeriod(value & 0x0f);
				   break;
		case 0x0d: // pass
				   break;
		case 0x0e: audio->setNoiseLoop((value & 0x80) == 0x80);
				   audio->setNoisePeriod(value & 0x0f);
				   break;
		case 0x0f: audio->setNoiseLength(value >> 3);
				   break;
		case 0x10: audio->setDeltaInterrupt((value & 0x80) == 0x80);
				   audio->setDeltaLoop((value & 0x40) == 0x40);
				   audio->setDeltaFrequency(value & 0x0f);
				   break;
		case 0x11: audio->setDeltaDirect(value & 0x7f);
				   break;
		case 0x12: audio->setDeltaAddress(value);
				   break;
		case 0x13: audio->setDeltaLength(value);
				   break;
		case 0x15: audio->setDeltaEnable((value & 0x10) == 0x10);
				   audio->setNoiseEnable((value & 0x08) == 0x08);
				   audio->setTriangleEnable((value & 0x04) == 0x04);
				   audio->setPulse2Enable((value & 0x02) == 0x02);
				   audio->setPulse1Enable((value & 0x01) == 0x01);
				   break;
		case 0x17: audio->setFrameSequence((value & 0x80) == 0x80);
				   audio->setFrameInterrupt((value & 0x40) == 0x40);
				   break;
		default: std::cout << "Invaled APU Address " << std::hex << int(address) << "\n";
				   break;
	}
}