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
		// mixing
		*out = 0;
		if (pulse1 != 0 || pulse2 != 0) *out += 95.88 / ((8128.0 / (pulse1 + pulse2)) + 100);
		if (triangle != 0 || noise != 0 || delta != 0) *out += 159.79 / ((1 / (triangle/8227.0 + noise/12241.0 + delta/22638.0)) + 100);
		*out = (*out * 2) - 1;
		if (*out > 1) *out = 1;
		if (*out < -1) *out = -1;
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

	e = Pa_OpenStream(
			&outputStream,
			NULL,
			outputParams,
			sampleRate,
			64,
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
	audioData->pulse1Length = length;
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
	audioData->pulse2Length = length;
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
	audioData->triangleLength = length;
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
	audioData->noiseLength = length;
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
}

void Audio::setPulse2Enable(bool enable){
	audioData->pulse2Enable = enable;
}

void Audio::setTriangleEnable(bool enable){
	audioData->triangleEnable = enable;
}

void Audio::setNoiseEnable(bool enable){
	audioData->noiseEnable = enable;
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

int main(){
	Audio* a = new Audio();
	auto now = std::chrono::high_resolution_clock::now();
	auto prev = std::chrono::high_resolution_clock::now();
	auto elapsed = std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1,1000>>>(now - prev);
	while (elapsed.count() < 1000){
		now = std::chrono::high_resolution_clock::now();
		elapsed = std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1,1000>>>(now - prev);
	}
	a->stopStream();
	return 0;
}

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
