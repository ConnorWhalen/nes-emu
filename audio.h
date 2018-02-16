#ifndef AUDIO_H
#define AUDIO_H

#include "portaudio.h"
#include <iostream>
#include <stdlib.h>

#include "cpu.h"

class CPU;

typedef struct{
} AudioData;

class Audio{
	public:
		Audio();
		void execute();
		void stopStream();

		void calculatePulse1Period();
		void updatePulse1Sweep();
		void calculatePulse1Envelope();
		void calculatePulse2Period();
		void updatePulse2Sweep();
		void calculatePulse2Envelope();

		void setPulse1Duty(unsigned char duty);
		void setPulse1Halt(bool halt);
		void setPulse1Envelope(bool envelope);
		void setPulse1EnvelopePeriod(unsigned char period);
		void setPulse1Sweep(bool sweep);
		void setPulse1SweepPeriod(unsigned char period);
		void setPulse1SweepNegative(bool negative);
		void setPulse1SweepShift(unsigned char shift);
		void setPulse1TimerHigh(unsigned char high);
		void setPulse1TimerLow(unsigned char low);
		void setPulse1Length(unsigned char length);

		void setPulse2Duty(unsigned char duty);
		void setPulse2Halt(bool halt);
		void setPulse2Envelope(bool envelope);
		void setPulse2EnvelopePeriod(unsigned char period);
		void setPulse2Sweep(bool sweep);
		void setPulse2SweepPeriod(unsigned char period);
		void setPulse2SweepNegative(bool negative);
		void setPulse2SweepShift(unsigned char shift);
		void setPulse2TimerHigh(unsigned char high);
		void setPulse2TimerLow(unsigned char low);
		void setPulse2Length(unsigned char length);

		void setTriangleHalt(bool halt);
		void setTriangleFrameCount(unsigned char frame);
		void setTriangleTimerHigh(unsigned char high);
		void setTriangleTimerLow(unsigned char low);
		void setTriangleLength(unsigned char length);

		void setNoiseHalt(bool halt);
		void setNoiseEnvelope(bool envelope);
		void setNoiseEnvelopePeriod(unsigned char period);
		void setNoiseLoop(bool loop);
		void setNoisePeriod(unsigned char period);
		void setNoiseLength(unsigned char length);

		void setDeltaInterrupt(bool interrupt);
		void setDeltaLoop(bool loop);
		void setDeltaFrequency(unsigned char frequency);
		void setDeltaDirect(unsigned char direct);
		void setDeltaAddress(unsigned char address);
		void setDeltaLength(unsigned char length);

		void setPulse1Enable(bool enable);
		void setPulse2Enable(bool enable);
		void setTriangleEnable(bool enable);
		void setNoiseEnable(bool enable);
		void setDeltaEnable(bool enable);
		void setFrameSequence(bool sequence);
		void setFrameInterrupt(bool interrupt);

		void setCpu(CPU* cpu);

		unsigned char getStatus();
	private:
		static constexpr int sampleRate = 44100;
		static constexpr int samplesPerFrame = 735;
		static constexpr int samplesPerQuarterFrame = 184;
		unsigned char getLength(unsigned char index);
		PaStreamParameters* outputParams;
		PaStream* outputStream;

		std::iostream* inputStream;

		bool* pulse1Duty;
		bool pulse1Halt;
		bool pulse1Envelope;
		unsigned char pulse1EnvelopePeriod;
		bool pulse1Sweep;
		unsigned char pulse1SweepPeriod;
		bool pulse1SweepNegative;
		unsigned char pulse1SweepShift;
		unsigned short pulse1Timer;
		unsigned char pulse1Length;

		unsigned short pulse1CurrentTimer;
		unsigned short pulse1CalculatedTimer;
		unsigned char pulse1Phase;
		bool pulse1SweepMute;
		bool pulse1StartFlag;
		bool pulse1SweepReload;
		unsigned char pulse1DecayCounter;
		unsigned char pulse1EnvelopeCurrentPeriod;
		unsigned char pulse1SweepCurrentPeriod;

		bool* pulse2Duty;
		bool pulse2Halt;
		bool pulse2Envelope;
		unsigned char pulse2EnvelopePeriod;
		bool pulse2Sweep;
		unsigned char pulse2SweepPeriod;
		bool pulse2SweepNegative;
		unsigned char pulse2SweepShift;
		unsigned short pulse2Timer;
		unsigned char pulse2Length;

		unsigned short pulse2CurrentTimer;
		unsigned short pulse2CalculatedTimer;
		unsigned char pulse2Phase;
		bool pulse2SweepMute;
		bool pulse2StartFlag;
		bool pulse2SweepReload;
		unsigned char pulse2DecayCounter;
		unsigned char pulse2EnvelopeCurrentPeriod;
		unsigned char pulse2SweepCurrentPeriod;

		bool triangleHalt;
		unsigned short triangleFrameCount;
		unsigned short triangleTimer;
		unsigned char triangleLength;

		bool noiseHalt;
		bool noiseEnvelope;
		unsigned char noiseEnvelopePeriod;
		bool noiseLoop;
		unsigned char noisePeriod;
		unsigned char noiseLength;

		bool deltaInterrupt;
		bool deltaLoop;
		unsigned char deltaFrequency;
		unsigned char deltaDirect;
		unsigned char deltaAddress;
		unsigned char deltaLength;

		bool pulse1Enable;
		bool pulse2Enable;
		bool triangleEnable;
		bool noiseEnable;
		bool deltaEnable;
		bool frameSequence;
		bool frameInterrupt;

		unsigned char frameCount;

		CPU* cpu;
};

#endif

// Supported standard sample rates
//  for half-duplex 16 bit 2 channel output = 
// 	 8000.00,  9600.00, 11025.00, 12000.00,
// 	16000.00, 22050.00, 24000.00, 32000.00,
// 	44100.00, 48000.00, 88200.00, 96000.00,
// 	192000.00