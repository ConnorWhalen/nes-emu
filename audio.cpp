#include "audio.h"

// portaudio callback which processes the next <samplesPerFrame> samples
static int paCallback(const void *inputBuffer, void *outputBuffer,
						  unsigned long samplesPerFrame,
						  const PaStreamCallbackTimeInfo* timeInfo,
						  PaStreamCallbackFlags statusFlags,
						  void *userData){
	AudioData *audioData = (AudioData*)userData;
	float *out = (float*)outputBuffer;
	(void) timeInfo;
	(void) statusFlags;
	(void) inputBuffer;

	unsigned char pulse1 = 0;
	unsigned char pulse2 = 0;
	unsigned char triangle = 0;
	unsigned char noise = 0;
	unsigned char delta = 0;
	
	for (int i = 0; i < samplesPerFrame; i++){
		// pulse 1 clocked ~ 20x audio sample rate
		// TODO: filtering downsampled signal, for now take last (20th) value
		for (int i = 0; i < 20; i++){
			// pulse 1
			if (audioData->pulse1CalculatedTimer > 7 && !audioData->pulse1Halt && audioData->pulse1Length != 0){
				// std::cout<< "pulse 1 enabled " << audioData->pulse1SweepMute << " " << int(audioData->pulse1Duty[audioData->pulse1Phase]) << "\n";
				if (audioData->pulse1CurrentTimer == 0){
					audioData->pulse1CurrentTimer = audioData->pulse1CalculatedTimer;
					if (++audioData->pulse1Phase == 8) audioData->pulse1Phase = 0;
				} else{
					audioData->pulse1CurrentTimer--;
				}
				if (!audioData->pulse1SweepMute && audioData->pulse1Duty[audioData->pulse1Phase]){
					// std::cout<< "determining pulse 1 output" << "\n";
					if (audioData->pulse1Envelope) pulse1 = audioData->pulse1EnvelopePeriod;
					else pulse1 = audioData->pulse1DecayCounter;
				} else pulse1 = 0;
			}

			// pulse 2
			if (audioData->pulse2CalculatedTimer > 7 && !audioData->pulse2Halt && audioData->pulse2Length != 0){
				// std::cout<< "pulse 2 enabled " << audioData->pulse2SweepMute << " " << int(audioData->pulse2Duty[audioData->pulse2Phase]) << "\n";
				if (audioData->pulse2CurrentTimer == 0){
					audioData->pulse2CurrentTimer = audioData->pulse2CalculatedTimer;
					if (++audioData->pulse2Phase == 8) audioData->pulse2Phase = 0;
				} else{
					audioData->pulse2CurrentTimer--;
				}
				if (!audioData->pulse2SweepMute && audioData->pulse2Duty[audioData->pulse2Phase]){
					// std::cout<< "determining pulse 2 output" << "\n";
					if (audioData->pulse2Envelope) pulse2 = audioData->pulse2EnvelopePeriod;
					else pulse2 = audioData->pulse2DecayCounter;
				} else pulse2 = 0;
			}
		}


		// mixing, may need to be approximated
		float mixed = 0;
		if (pulse1 != 0 || pulse2 != 0) mixed += 95.88 / ((8128.0 / (pulse1 + pulse2)) + 100);
		if (triangle != 0 || noise != 0 || delta != 0) mixed += 159.79 / ((1 / (triangle/8227.0 + noise/12241.0 + delta/22638.0)) + 100);
		*out++ = mixed;
		// std::cout<< mixed << "\n";
	}

	// Frame counter
	audioData->frameCount++;
	if (audioData->frameSequence){
		// 5 step sequence
		if (audioData->frameCount > 4) audioData->frameCount = 0;

		if (audioData->frameCount == 1 || audioData->frameCount == 4){
			if (audioData->pulse1Length != 0) audioData->pulse1Length--;
			if (audioData->pulse2Length != 0) audioData->pulse2Length--;
			if (audioData->triangleLength != 0) audioData->triangleLength--;
			if (audioData->noiseLength != 0) audioData->noiseLength--;
			updatePulse1Sweep(audioData);
			updatePulse2Sweep(audioData);
		}

		if (audioData->frameCount != 3){
			calculatePulse1Envelope(audioData);
			calculatePulse2Envelope(audioData);
		}
	}else{
		// 4 step sequence
		if (audioData->frameCount > 3) audioData->frameCount = 0;

		if (audioData->frameCount == 1 || audioData->frameCount == 3){
			if (audioData->pulse1Length != 0) audioData->pulse1Length--;
			if (audioData->pulse2Length != 0) audioData->pulse2Length--;
			if (audioData->triangleLength != 0) audioData->triangleLength--;
			if (audioData->noiseLength != 0) audioData->noiseLength--;
			updatePulse1Sweep(audioData);
			updatePulse2Sweep(audioData);
		}

		calculatePulse1Envelope(audioData);
		calculatePulse2Envelope(audioData);

		if (audioData->frameInterrupt){
			if (audioData->cpu != NULL){
				audioData->cpu->irq();
			}
		}
	}
	
	return paContinue;
}

Audio::Audio(){
	PaError e = Pa_Initialize();
	if (e != paNoError){
		Pa_Terminate();
		std::cout << "Portaudio Error Number " << e << ": " << Pa_GetErrorText(e) << "\n";
	}

	outputParams = new PaStreamParameters();
	outputParams->device = Pa_GetDefaultOutputDevice();
	if (outputParams->device == paNoDevice) {
		Pa_Terminate();
		std::cout << "Portaudio Error: No default output device" << "\n";
	}
	outputParams->channelCount = 1;
	outputParams->sampleFormat = paFloat32;
	outputParams->suggestedLatency = Pa_GetDeviceInfo(outputParams->device)->defaultLowOutputLatency;
	outputParams->hostApiSpecificStreamInfo = NULL;

	audioData = new AudioData();

	setPulse1Duty(0);
	audioData->pulse1Halt = false;
	audioData->pulse1Envelope = false;
	audioData->pulse1EnvelopePeriod = 0;
	audioData->pulse1Sweep = false;
	audioData->pulse1SweepPeriod = 0;
	audioData->pulse1SweepNegative = false;
	audioData->pulse1SweepShift = 0;
	audioData->pulse1Timer = 0;
	audioData->pulse1Length = 0;

	audioData->pulse1CurrentTimer = 0;
	audioData->pulse1CalculatedTimer = 0;
	audioData->pulse1Phase = 0;
	audioData->pulse1SweepMute = false;
	audioData->pulse1StartFlag = false;
	audioData->pulse1SweepReload = false;
	audioData->pulse1DecayCounter = 0;
	audioData->pulse1EnvelopeCurrentPeriod = 0;
	audioData->pulse1SweepCurrentPeriod = 0;

	setPulse2Duty(0);
	audioData->pulse2Halt = false;
	audioData->pulse2Envelope = false;
	audioData->pulse2EnvelopePeriod = 0;
	audioData->pulse2Sweep = false;
	audioData->pulse2SweepPeriod = 0;
	audioData->pulse2SweepNegative = false;
	audioData->pulse2SweepShift = 0;
	audioData->pulse2Timer = 0;
	audioData->pulse2Length = 0;

	audioData->pulse2CurrentTimer = 0;
	audioData->pulse2CalculatedTimer = 0;
	audioData->pulse2Phase = 0;
	audioData->pulse2SweepMute = false;
	audioData->pulse2StartFlag = false;
	audioData->pulse2SweepReload = false;
	audioData->pulse2DecayCounter = 0;
	audioData->pulse2EnvelopeCurrentPeriod = 0;
	audioData->pulse2SweepCurrentPeriod = 0;

	audioData->triangleHalt = false;
	audioData->triangleFrameCount = 0;
	audioData->triangleTimer = 0;
	audioData->triangleLength = 0;

	audioData->noiseHalt = false;
	audioData->noiseEnvelope = false;
	audioData->noiseEnvelopePeriod = 0;
	audioData->noiseLoop = false;
	audioData->noisePeriod = 0;
	audioData->noiseLength = 0;

	audioData->deltaInterrupt = false;
	audioData->deltaLoop = false;
	audioData->deltaFrequency = 0;
	audioData->deltaDirect = 0;
	audioData->deltaAddress = 0;
	audioData->deltaLength = 0;

	audioData->pulse1Enable = false;
	audioData->pulse2Enable = false;
	audioData->triangleEnable = false;
	audioData->noiseEnable = false;
	audioData->deltaEnable = false;
	audioData->frameSequence = false;
	audioData->frameInterrupt = false;

	audioData->frameCount = 0;

	audioData->cpu = NULL;

	e = Pa_OpenStream(
			&outputStream,
			NULL,
			outputParams,
			sampleRate,
			samplesPerQuarterFrame,
			paClipOff,
			paCallback,
			audioData);
	if (e != paNoError){
		Pa_Terminate();
		std::cout << "Portaudio Error Number " << e << ": " << Pa_GetErrorText(e) << "\n";
	}

	e = Pa_StartStream(outputStream);
	if(e != paNoError){
		Pa_Terminate();
		std::cout << "Portaudio Error Number " << e << ": " << Pa_GetErrorText(e) << "\n";
	}
}

void Audio::stopStream(){
	PaError e = Pa_StopStream(outputStream);
	if (e != paNoError){
		Pa_Terminate();
		std::cout << "Portaudio Error Number " << e << ": " << Pa_GetErrorText(e) << "\n";
	}
}

void calculatePulse1Period(AudioData* audioData){
	audioData->pulse1CalculatedTimer = audioData->pulse1Timer;
	short pulse1PeriodOffset = audioData->pulse1Timer >> audioData->pulse1SweepShift;
	if (audioData->pulse1SweepNegative) pulse1PeriodOffset = -pulse1PeriodOffset;
	// std::cout << int(audioData->pulse1CalculatedTimer) << " " << int(pulse1PeriodOffset) << "\n";
	if (audioData->pulse1CalculatedTimer + pulse1PeriodOffset > 0x7ff){
		audioData->pulse1SweepMute = true;
	}
	else{
		audioData->pulse1SweepMute = false;
		if (audioData->pulse1Sweep){
			audioData->pulse1CalculatedTimer += pulse1PeriodOffset;
		}
	}
	// std::cout<< "calculating pulse 1 period = " << audioData->pulse1CalculatedTimer << "\n";
}

void updatePulse1Sweep(AudioData* audioData){
	// std::cout<< "updating pulse 1 sweep" << "\n";
	if (audioData->pulse1SweepReload){
		audioData->pulse1SweepReload = false;
		if (audioData->pulse1SweepCurrentPeriod == 0){
			calculatePulse1Period(audioData);
		}
		audioData->pulse1SweepCurrentPeriod = audioData->pulse1SweepPeriod;
	} else if (audioData->pulse1SweepCurrentPeriod > 0){
		audioData->pulse1SweepCurrentPeriod--;
	} else {
		audioData->pulse1SweepCurrentPeriod = audioData->pulse1SweepPeriod;
		calculatePulse1Period(audioData);
	}
}

void calculatePulse1Envelope(AudioData* audioData){
	if (audioData->pulse1StartFlag){
		audioData->pulse1StartFlag = false;
		audioData->pulse1DecayCounter = 15;
		audioData->pulse1EnvelopeCurrentPeriod = audioData->pulse1EnvelopePeriod;
	} else{
		if (--audioData->pulse1EnvelopeCurrentPeriod == 0xff){
			audioData->pulse1EnvelopeCurrentPeriod = audioData->pulse1EnvelopePeriod;
			if (audioData->pulse1DecayCounter > 0){
				audioData->pulse1DecayCounter--;
			} else if (audioData->pulse1Halt){
				audioData->pulse1DecayCounter = 15;
			}
		}
	}
}

void calculatePulse2Period(AudioData* audioData){
	audioData->pulse2CalculatedTimer = audioData->pulse2Timer;
	short pulse2PeriodOffset = audioData->pulse2Timer >> audioData->pulse2SweepShift;
	if (audioData->pulse2SweepNegative) pulse2PeriodOffset = -pulse2PeriodOffset;
	// std::cout << int(audioData->pulse2CalculatedTimer) << " " << int(pulse2PeriodOffset) << "\n";
	if (audioData->pulse2CalculatedTimer + pulse2PeriodOffset > 0x7ff){
		audioData->pulse2SweepMute = true;
	}
	else{
		audioData->pulse2SweepMute = false;
		if (audioData->pulse2Sweep){
			audioData->pulse2CalculatedTimer += pulse2PeriodOffset;
		}
	}
	// std::cout<< "calculating pulse 2 period = " << audioData->pulse2CalculatedTimer << "\n";
}

void updatePulse2Sweep(AudioData* audioData){
	// std::cout<< "updating pulse 2 sweep" << "\n";
	if (audioData->pulse2SweepReload){
		audioData->pulse2SweepReload = false;
		if (audioData->pulse2SweepCurrentPeriod == 0){
			calculatePulse2Period(audioData);
		}
		audioData->pulse2SweepCurrentPeriod = audioData->pulse2SweepPeriod;
	} else if (audioData->pulse2SweepCurrentPeriod > 0){
		audioData->pulse2SweepCurrentPeriod--;
	} else {
		audioData->pulse2SweepCurrentPeriod = audioData->pulse2SweepPeriod;
		calculatePulse2Period(audioData);
	}
}

void calculatePulse2Envelope(AudioData* audioData){
	if (audioData->pulse2StartFlag){
		audioData->pulse2StartFlag = false;
		audioData->pulse2DecayCounter = 15;
		audioData->pulse2EnvelopeCurrentPeriod = audioData->pulse2EnvelopePeriod;
	} else{
		if (--audioData->pulse2EnvelopeCurrentPeriod == 0xff){
			audioData->pulse2EnvelopeCurrentPeriod = audioData->pulse2EnvelopePeriod;
			if (audioData->pulse2DecayCounter > 0){
				audioData->pulse2DecayCounter--;
			} else if (audioData->pulse2Halt){
				audioData->pulse2DecayCounter = 15;
			}
		}
	}
}

void Audio::setPulse1Duty(unsigned char duty){
	if (duty == 0) audioData->pulse1Duty = new bool[8] {false, true, false, false, false, false, false, false};
	else if (duty == 1) audioData->pulse1Duty = new bool[8] {false, true, true, false, false, false, false, false};
	else if (duty == 2) audioData->pulse1Duty = new bool[8] {false, true, true, true, true, false, false, false};
	else if (duty == 3) audioData->pulse1Duty = new bool[8] {true, false, false, true, true, true, true, true};
	else audioData->pulse1Duty = new bool[8] {false, false, false, false, false, false, false, false};
}

void Audio::setPulse1Halt(bool halt){
	audioData->pulse1Halt = halt;
}

void Audio::setPulse1Envelope(bool envelope){
	audioData->pulse1Envelope = envelope;
}

void Audio::setPulse1EnvelopePeriod(unsigned char period){
	audioData->pulse1EnvelopePeriod = period;
}

void Audio::setPulse1Sweep(bool sweep){
	audioData->pulse1Sweep = sweep;
}

void Audio::setPulse1SweepPeriod(unsigned char period){
	audioData->pulse1SweepPeriod = period;
	audioData->pulse1SweepReload = true;
}

void Audio::setPulse1SweepNegative(bool negative){
	audioData->pulse1SweepNegative = negative;
}

void Audio::setPulse1SweepShift(unsigned char shift){
	audioData->pulse1SweepShift = shift;
}

void Audio::setPulse1TimerHigh(unsigned char high){
	audioData->pulse1Timer = (high << 8) + (audioData->pulse1Timer & 0x00ff);
}

void Audio::setPulse1TimerLow(unsigned char low){
	audioData->pulse1Timer = low + (audioData->pulse1Timer & 0xff00);
}

void Audio::setPulse1Length(unsigned char length){
	if (audioData->pulse1Enable) audioData->pulse1Length = getLength(length);
	audioData->pulse1Phase = 0;
	audioData->pulse1StartFlag = true;
}

void Audio::setPulse2Duty(unsigned char duty){
	if (duty == 0) audioData->pulse2Duty = new bool[8] {false, true, false, false, false, false, false, false};
	else if (duty == 1) audioData->pulse2Duty = new bool[8] {false, true, true, false, false, false, false, false};
	else if (duty == 2) audioData->pulse2Duty = new bool[8] {false, true, true, true, true, false, false, false};
	else if (duty == 3) audioData->pulse2Duty = new bool[8] {true, false, false, true, true, true, true, true};
	else audioData->pulse2Duty = new bool[8] {false, false, false, false, false, false, false, false};
}

void Audio::setPulse2Halt(bool halt){
	audioData->pulse2Halt = halt;
}

void Audio::setPulse2Envelope(bool envelope){
	audioData->pulse2Envelope = envelope;
}

void Audio::setPulse2EnvelopePeriod(unsigned char period){
	audioData->pulse2EnvelopePeriod = period;
}

void Audio::setPulse2Sweep(bool sweep){
	audioData->pulse2Sweep = sweep;
}

void Audio::setPulse2SweepPeriod(unsigned char period){
	audioData->pulse2SweepPeriod = period;
	audioData->pulse2SweepReload = true;
}

void Audio::setPulse2SweepNegative(bool negative){
	audioData->pulse2SweepNegative = negative;
}

void Audio::setPulse2SweepShift(unsigned char shift){
	audioData->pulse2SweepShift = shift;
}

void Audio::setPulse2TimerHigh(unsigned char high){
	audioData->pulse2Timer = (high << 8) + (audioData->pulse2Timer & 0x00ff);
}

void Audio::setPulse2TimerLow(unsigned char low){
	audioData->pulse2Timer = low + (audioData->pulse2Timer & 0xff00);
}

void Audio::setPulse2Length(unsigned char length){
	if (audioData->pulse2Enable) audioData->pulse2Length = getLength(length);
	audioData->pulse2Phase = 0;
	audioData->pulse2StartFlag = true;
}

void Audio::setTriangleHalt(bool halt){
	audioData->triangleHalt = halt;
}

void Audio::setTriangleFrameCount(unsigned char frame){
	audioData->triangleFrameCount = frame;
}

void Audio::setTriangleTimerHigh(unsigned char high){
	audioData->triangleTimer = (high << 8) + (audioData->triangleTimer & 0x00ff);
}

void Audio::setTriangleTimerLow(unsigned char low){
	audioData->triangleTimer = low + (audioData->triangleTimer & 0xff00);
}

void Audio::setTriangleLength(unsigned char length){
	if (audioData->triangleEnable) audioData->triangleLength = getLength(length);
}

void Audio::setNoiseHalt(bool halt){
	audioData->noiseHalt = halt;
}

void Audio::setNoiseEnvelope(bool envelope){
	audioData->noiseEnvelope = envelope;
}

void Audio::setNoiseEnvelopePeriod(unsigned char period){
	audioData->noiseEnvelopePeriod = period;
}

void Audio::setNoiseLoop(bool loop){
	audioData->noiseLoop = loop;
}

void Audio::setNoisePeriod(unsigned char period){
	audioData->noisePeriod = period;
}

void Audio::setNoiseLength(unsigned char length){
	if (audioData->noiseEnable) audioData->noiseLength = getLength(length);
}

void Audio::setDeltaInterrupt(bool interrupt){
	audioData->deltaInterrupt = interrupt;
}

void Audio::setDeltaLoop(bool loop){
	audioData->deltaLoop = loop;
}

void Audio::setDeltaFrequency(unsigned char frequency){
	audioData->deltaFrequency = frequency;
}

void Audio::setDeltaDirect(unsigned char direct){
	audioData->deltaDirect = direct;
}

void Audio::setDeltaAddress(unsigned char address){
	audioData->deltaAddress = address;
}

void Audio::setDeltaLength(unsigned char length){
	audioData->deltaLength = length;
}

void Audio::setPulse1Enable(bool enable){
	audioData->pulse1Enable = enable;
	if (!enable) audioData->pulse1Length = 0;
}

void Audio::setPulse2Enable(bool enable){
	audioData->pulse2Enable = enable;
	if (!enable) audioData->pulse2Length = 0;
}

void Audio::setTriangleEnable(bool enable){
	audioData->triangleEnable = enable;
	if (!enable) audioData->triangleLength = 0;
}

void Audio::setNoiseEnable(bool enable){
	audioData->noiseEnable = enable;
	if (!enable) audioData->noiseLength = 0;
}

void Audio::setDeltaEnable(bool enable){
	audioData->deltaEnable = enable;
}

void Audio::setFrameSequence(bool sequence){
	audioData->frameSequence = sequence;
	audioData->frameCount = 0;
}

void Audio::setFrameInterrupt(bool interrupt){
	std::cout << "setting frame interrupt enable to " << int(interrupt) << "\n";
	audioData->frameInterrupt = interrupt;
}

void Audio::setCpu(CPU* cpu){
	audioData->cpu = cpu;
}

unsigned char Audio::getStatus(){
	// TODO: return APU status
	return 0;
}

// int main(){
// 	Audio* a = new Audio();
// 	a->setPulse2Enable(true);
// 	a->setPulse2Duty(2);
// 	a->setPulse2EnvelopePeriod(8);
// 	a->setPulse2Envelope(true);
// 	a->setPulse2Sweep(false);
// 	a->setPulse2SweepNegative(true);
// 	a->setPulse2TimerHigh(0);
// 	a->setPulse2TimerLow(127);
// 	a->setPulse2Length(1);
// 	auto now = std::chrono::high_resolution_clock::now();
// 	auto prev = std::chrono::high_resolution_clock::now();
// 	auto elapsed = std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1,1000>>>(now - prev);
// 	while (elapsed.count() < 1000){
// 		now = std::chrono::high_resolution_clock::now();
// 		elapsed = std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1,1000>>>(now - prev);
// 	}

// 	a->setPulse2TimerHigh(0);
// 	a->setPulse2TimerLow(150);
// 	a->setPulse2Length(1);
// 	now = std::chrono::high_resolution_clock::now();
// 	prev = std::chrono::high_resolution_clock::now();
// 	elapsed = std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1,1000>>>(now - prev);
// 	while (elapsed.count() < 4000){
// 		now = std::chrono::high_resolution_clock::now();
// 		elapsed = std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1,1000>>>(now - prev);
// 	}
// 	a->stopStream();
// 	return 0;
// }

unsigned char Audio::getLength(unsigned char index){
	unsigned char ret = 1;
	switch (index){
		case 0x00: ret = 10;
				   break;
		case 0x01: ret = 254;
				   break;
		case 0x02: ret = 20;
				   break;
		case 0x03: ret = 2;
				   break;
		case 0x04: ret = 40;
				   break;
		case 0x05: ret = 4;
				   break;
		case 0x06: ret = 80;
				   break;
		case 0x07: ret = 6;
				   break;
		case 0x08: ret = 160;
				   break;
		case 0x09: ret = 8;
				   break;
		case 0x0a: ret = 60;
				   break;
		case 0x0b: ret = 10;
				   break;
		case 0x0c: ret = 14;
				   break;
		case 0x0d: ret = 12;
				   break;
		case 0x0e: ret = 26;
				   break;
		case 0x0f: ret = 14;
				   break;
		case 0x10: ret = 12;
				   break;
		case 0x11: ret = 16;
				   break;
		case 0x12: ret = 24;
				   break;
		case 0x13: ret = 18;
				   break;
		case 0x14: ret = 48;
				   break;
		case 0x15: ret = 20;
				   break;
		case 0x16: ret = 96;
				   break;
		case 0x17: ret = 22;
				   break;
		case 0x18: ret = 192;
				   break;
		case 0x19: ret = 24;
				   break;
		case 0x1a: ret = 72;
				   break;
		case 0x1b: ret = 26;
				   break;
		case 0x1c: ret = 16;
				   break;
		case 0x1d: ret = 28;
				   break;
		case 0x1e: ret = 32;
				   break;
		case 0x1f: ret = 30;
				   break;
		default: std::cout << "Invalid sound length index " << std::hex << int(index) << "\n";
				   break;
	}
	return ret - 1;
}
