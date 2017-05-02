#include "audio.h"

// portaudio callback which processes the next <framesPerBuffer> samples
static int paCallback(const void *inputBuffer, void *outputBuffer,
						  unsigned long framesPerBuffer,
						  const PaStreamCallbackTimeInfo* timeInfo,
						  PaStreamCallbackFlags statusFlags,
						  void *userData){
	AudioData *data = (AudioData*)userData;
	float *out = (float*)outputBuffer;
	(void) timeInfo;
	(void) statusFlags;
	(void) inputBuffer;

	unsigned char pulse1 = 0;
	unsigned char pulse2 = 0;
	unsigned char triangle = 0;
	unsigned char noise = 0;
	unsigned char delta = 0;
	
	for (int i=0; i < framesPerBuffer; i++){
		if (audioData->pulse1Timer > 7 && !audioData->pulse1Halt && audioData->pulse1Length != 0){
			if (--audioData->pulse1CurrentTimer == -1){
				audioData->pulse1CurrentTimer = audioData->pulse1Timer;
				if (++audioData->pulse1Phase == 8) audioData->pulse1Phase = 0;
			}
			if (audioData->pulse1Duty[audioData->pulse1Phase]) pulse1 = 1;
		}


		// mixing
		*out = 0;
		if (pulse1 != 0 || pulse2 != 0) *out += 95.88 / ((8128.0 / (pulse1 + pulse2)) + 100);
		if (triangle != 0 || noise != 0 || delta != 0) *out += 159.79 / ((1 / (triangle/8227.0 + noise/12241.0 + delta/22638.0)) + 100);
		*out = (*out * 2) - 1;
		if (*out > 1) *out = 1;
		if (*out < -1) *out = -1;
	}

	if (audioData->pulse1Length != 0) audioData->pulse1Length--;
	if (audioData->pulse2Length != 0) audioData->pulse2Length--;
	if (audioData->triangleLength != 0) audioData->triangleLength--;
	if (audioData->noiseLength != 0) audioData->noiseLength--;
	
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
	audioData->pulse1Phase = 0;

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
	audioData->frameTimer = 0;
	audioData->pulse1Time = 0;

	e = Pa_OpenStream(
			&outputStream,
			NULL,
			outputParams,
			sampleRate,
			samplesPerFrame,
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

void Audio::setPulse1Duty(unsigned char duty){
	if (duty == 0) audioData->pulse1Duty = new bool[8] {false, true, false, false, false, false, false, false};
	if (duty == 1) audioData->pulse1Duty = new bool[8] {false, true, true, false, false, false, false, false};
	if (duty == 2) audioData->pulse1Duty = new bool[8] {false, true, true, true, true, false, false, false};
	if (duty == 3) audioData->pulse1Duty = new bool[8] {true, false, false, true, true, true, true, true};
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
}

void Audio::setPulse2Duty(unsigned char duty){
	if (duty == 0) audioData->pulse2Duty = new bool[8] {false, true, false, false, false, false, false, false};
	if (duty == 1) audioData->pulse2Duty = new bool[8] {false, true, true, false, false, false, false, false};
	if (duty == 2) audioData->pulse2Duty = new bool[8] {false, true, true, true, true, false, false, false};
	if (duty == 3) audioData->pulse2Duty = new bool[8] {true, false, false, true, true, true, true, true};
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
}

void Audio::setFrameInterrupt(bool interrupt){
	audioData->frameInterrupt = interrupt;
}

unsigned char Audio::getStatus(){
	// TODO: return APU status
	return 0;
}

// int main(){
// 	Audio* a = new Audio();
// 	auto now = std::chrono::high_resolution_clock::now();
// 	auto prev = std::chrono::high_resolution_clock::now();
// 	auto elapsed = std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1,1000>>>(now - prev);
// 	while (elapsed.count() < 1000){
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
