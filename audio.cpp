#include "audio.h"

// portaudio callback which processes the next <samplesPerFrame> samples
static int paCallback(const void *inputBuffer, void *outputBuffer,
						  unsigned long samplesPerFrame,
						  const PaStreamCallbackTimeInfo* timeInfo,
						  PaStreamCallbackFlags statusFlags,
						  void *userData){
	float *in = (float*)userData;
	float *out = (float*)outputBuffer;
	(void) timeInfo;
	(void) statusFlags;
	(void) inputBuffer;

	for (int i = 0; i < samplesPerFrame; i++){
		in >> *out++;
	}
	
	return paContinue;
}

void Audio::execute(){
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
			if (pulse1CalculatedTimer > 7 && !pulse1Halt && pulse1Length != 0){
				// std::cout<< "pulse 1 enabled " << pulse1SweepMute << " " << int(pulse1Duty[pulse1Phase]) << "\n";
				if (pulse1CurrentTimer == 0){
					pulse1CurrentTimer = pulse1CalculatedTimer;
					if (++pulse1Phase == 8) pulse1Phase = 0;
				} else{
					pulse1CurrentTimer--;
				}
				if (!pulse1SweepMute && pulse1Duty[pulse1Phase]){
					// std::cout<< "determining pulse 1 output" << "\n";
					if (pulse1Envelope) pulse1 = pulse1EnvelopePeriod;
					else pulse1 = pulse1DecayCounter;
				} else pulse1 = 0;
			}

			// pulse 2
			if (pulse2CalculatedTimer > 7 && !pulse2Halt && pulse2Length != 0){
				// std::cout<< "pulse 2 enabled " << pulse2SweepMute << " " << int(pulse2Duty[pulse2Phase]) << "\n";
				if (pulse2CurrentTimer == 0){
					pulse2CurrentTimer = pulse2CalculatedTimer;
					if (++pulse2Phase == 8) pulse2Phase = 0;
				} else{
					pulse2CurrentTimer--;
				}
				if (!pulse2SweepMute && pulse2Duty[pulse2Phase]){
					// std::cout<< "determining pulse 2 output" << "\n";
					if (pulse2Envelope) pulse2 = pulse2EnvelopePeriod;
					else pulse2 = pulse2DecayCounter;
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
	frameCount++;
	if (frameSequence){
		// 5 step sequence
		if (frameCount > 4) frameCount = 0;

		if (frameCount == 1 || frameCount == 4){
			if (pulse1Length != 0) pulse1Length--;
			if (pulse2Length != 0) pulse2Length--;
			if (triangleLength != 0) triangleLength--;
			if (noiseLength != 0) noiseLength--;
			updatePulse1Sweep(audioData);
			updatePulse2Sweep(audioData);
		}

		if (frameCount != 3){
			calculatePulse1Envelope(audioData);
			calculatePulse2Envelope(audioData);
		}
	}else{
		// 4 step sequence
		if (frameCount > 3) frameCount = 0;

		if (frameCount == 1 || frameCount == 3){
			if (pulse1Length != 0) pulse1Length--;
			if (pulse2Length != 0) pulse2Length--;
			if (triangleLength != 0) triangleLength--;
			if (noiseLength != 0) noiseLength--;
			updatePulse1Sweep(audioData);
			updatePulse2Sweep(audioData);
		}

		calculatePulse1Envelope(audioData);
		calculatePulse2Envelope(audioData);

		if (frameInterrupt){
			if (cpu != NULL){
				cpu->irq();
			}
		}
	}
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

	inputStream = new std::iostream();

	setPulse1Duty(0);
	pulse1Halt = false;
	pulse1Envelope = false;
	pulse1EnvelopePeriod = 0;
	pulse1Sweep = false;
	pulse1SweepPeriod = 0;
	pulse1SweepNegative = false;
	pulse1SweepShift = 0;
	pulse1Timer = 0;
	pulse1Length = 0;

	pulse1CurrentTimer = 0;
	pulse1CalculatedTimer = 0;
	pulse1Phase = 0;
	pulse1SweepMute = false;
	pulse1StartFlag = false;
	pulse1SweepReload = false;
	pulse1DecayCounter = 0;
	pulse1EnvelopeCurrentPeriod = 0;
	pulse1SweepCurrentPeriod = 0;

	setPulse2Duty(0);
	pulse2Halt = false;
	pulse2Envelope = false;
	pulse2EnvelopePeriod = 0;
	pulse2Sweep = false;
	pulse2SweepPeriod = 0;
	pulse2SweepNegative = false;
	pulse2SweepShift = 0;
	pulse2Timer = 0;
	pulse2Length = 0;

	pulse2CurrentTimer = 0;
	pulse2CalculatedTimer = 0;
	pulse2Phase = 0;
	pulse2SweepMute = false;
	pulse2StartFlag = false;
	pulse2SweepReload = false;
	pulse2DecayCounter = 0;
	pulse2EnvelopeCurrentPeriod = 0;
	pulse2SweepCurrentPeriod = 0;

	triangleHalt = false;
	triangleFrameCount = 0;
	triangleTimer = 0;
	triangleLength = 0;

	noiseHalt = false;
	noiseEnvelope = false;
	noiseEnvelopePeriod = 0;
	noiseLoop = false;
	noisePeriod = 0;
	noiseLength = 0;

	deltaInterrupt = false;
	deltaLoop = false;
	deltaFrequency = 0;
	deltaDirect = 0;
	deltaAddress = 0;
	deltaLength = 0;

	pulse1Enable = false;
	pulse2Enable = false;
	triangleEnable = false;
	noiseEnable = false;
	deltaEnable = false;
	frameSequence = false;
	frameInterrupt = false;

	frameCount = 0;

	cpu = NULL;

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

void Audio::calculatePulse1Period(){
	pulse1CalculatedTimer = pulse1Timer;
	short pulse1PeriodOffset = pulse1Timer >> pulse1SweepShift;
	if (pulse1SweepNegative) pulse1PeriodOffset = -pulse1PeriodOffset;
	// std::cout << int(pulse1CalculatedTimer) << " " << int(pulse1PeriodOffset) << "\n";
	if (pulse1CalculatedTimer + pulse1PeriodOffset > 0x7ff){
		pulse1SweepMute = true;
	}
	else{
		pulse1SweepMute = false;
		if (pulse1Sweep){
			pulse1CalculatedTimer += pulse1PeriodOffset;
		}
	}
	// std::cout<< "calculating pulse 1 period = " << pulse1CalculatedTimer << "\n";
}

void Audio::updatePulse1Sweep(){
	// std::cout<< "updating pulse 1 sweep" << "\n";
	if (pulse1SweepReload){
		pulse1SweepReload = false;
		if (pulse1SweepCurrentPeriod == 0){
			calculatePulse1Period(audioData);
		}
		pulse1SweepCurrentPeriod = pulse1SweepPeriod;
	} else if (pulse1SweepCurrentPeriod > 0){
		pulse1SweepCurrentPeriod--;
	} else {
		pulse1SweepCurrentPeriod = pulse1SweepPeriod;
		calculatePulse1Period(audioData);
	}
}

void Audio::calculatePulse1Envelope(){
	if (pulse1StartFlag){
		pulse1StartFlag = false;
		pulse1DecayCounter = 15;
		pulse1EnvelopeCurrentPeriod = pulse1EnvelopePeriod;
	} else{
		if (--pulse1EnvelopeCurrentPeriod == 0xff){
			pulse1EnvelopeCurrentPeriod = pulse1EnvelopePeriod;
			if (pulse1DecayCounter > 0){
				pulse1DecayCounter--;
			} else if (pulse1Halt){
				pulse1DecayCounter = 15;
			}
		}
	}
}

void Audio::calculatePulse2Period(){
	pulse2CalculatedTimer = pulse2Timer;
	short pulse2PeriodOffset = pulse2Timer >> pulse2SweepShift;
	if (pulse2SweepNegative) pulse2PeriodOffset = -pulse2PeriodOffset;
	// std::cout << int(pulse2CalculatedTimer) << " " << int(pulse2PeriodOffset) << "\n";
	if (pulse2CalculatedTimer + pulse2PeriodOffset > 0x7ff){
		pulse2SweepMute = true;
	}
	else{
		pulse2SweepMute = false;
		if (pulse2Sweep){
			pulse2CalculatedTimer += pulse2PeriodOffset;
		}
	}
	// std::cout<< "calculating pulse 2 period = " << pulse2CalculatedTimer << "\n";
}

void Audio::updatePulse2Sweep(){
	// std::cout<< "updating pulse 2 sweep" << "\n";
	if (pulse2SweepReload){
		pulse2SweepReload = false;
		if (pulse2SweepCurrentPeriod == 0){
			calculatePulse2Period(audioData);
		}
		pulse2SweepCurrentPeriod = pulse2SweepPeriod;
	} else if (pulse2SweepCurrentPeriod > 0){
		pulse2SweepCurrentPeriod--;
	} else {
		pulse2SweepCurrentPeriod = pulse2SweepPeriod;
		calculatePulse2Period(audioData);
	}
}

void Audio::calculatePulse2Envelope(){
	if (pulse2StartFlag){
		pulse2StartFlag = false;
		pulse2DecayCounter = 15;
		pulse2EnvelopeCurrentPeriod = pulse2EnvelopePeriod;
	} else{
		if (--pulse2EnvelopeCurrentPeriod == 0xff){
			pulse2EnvelopeCurrentPeriod = pulse2EnvelopePeriod;
			if (pulse2DecayCounter > 0){
				pulse2DecayCounter--;
			} else if (pulse2Halt){
				pulse2DecayCounter = 15;
			}
		}
	}
}

void Audio::setPulse1Duty(unsigned char duty){
	if (duty == 0) pulse1Duty = new bool[8] {false, true, false, false, false, false, false, false};
	else if (duty == 1) pulse1Duty = new bool[8] {false, true, true, false, false, false, false, false};
	else if (duty == 2) pulse1Duty = new bool[8] {false, true, true, true, true, false, false, false};
	else if (duty == 3) pulse1Duty = new bool[8] {true, false, false, true, true, true, true, true};
	else pulse1Duty = new bool[8] {false, false, false, false, false, false, false, false};
}

void Audio::setPulse1Halt(bool halt){
	pulse1Halt = halt;
}

void Audio::setPulse1Envelope(bool envelope){
	pulse1Envelope = envelope;
}

void Audio::setPulse1EnvelopePeriod(unsigned char period){
	pulse1EnvelopePeriod = period;
}

void Audio::setPulse1Sweep(bool sweep){
	pulse1Sweep = sweep;
}

void Audio::setPulse1SweepPeriod(unsigned char period){
	pulse1SweepPeriod = period;
	pulse1SweepReload = true;
}

void Audio::setPulse1SweepNegative(bool negative){
	pulse1SweepNegative = negative;
}

void Audio::setPulse1SweepShift(unsigned char shift){
	pulse1SweepShift = shift;
}

void Audio::setPulse1TimerHigh(unsigned char high){
	pulse1Timer = (high << 8) + (pulse1Timer & 0x00ff);
}

void Audio::setPulse1TimerLow(unsigned char low){
	pulse1Timer = low + (pulse1Timer & 0xff00);
}

void Audio::setPulse1Length(unsigned char length){
	if (pulse1Enable) pulse1Length = getLength(length);
	pulse1Phase = 0;
	pulse1StartFlag = true;
}

void Audio::setPulse2Duty(unsigned char duty){
	if (duty == 0) pulse2Duty = new bool[8] {false, true, false, false, false, false, false, false};
	else if (duty == 1) pulse2Duty = new bool[8] {false, true, true, false, false, false, false, false};
	else if (duty == 2) pulse2Duty = new bool[8] {false, true, true, true, true, false, false, false};
	else if (duty == 3) pulse2Duty = new bool[8] {true, false, false, true, true, true, true, true};
	else pulse2Duty = new bool[8] {false, false, false, false, false, false, false, false};
}

void Audio::setPulse2Halt(bool halt){
	pulse2Halt = halt;
}

void Audio::setPulse2Envelope(bool envelope){
	pulse2Envelope = envelope;
}

void Audio::setPulse2EnvelopePeriod(unsigned char period){
	pulse2EnvelopePeriod = period;
}

void Audio::setPulse2Sweep(bool sweep){
	pulse2Sweep = sweep;
}

void Audio::setPulse2SweepPeriod(unsigned char period){
	pulse2SweepPeriod = period;
	pulse2SweepReload = true;
}

void Audio::setPulse2SweepNegative(bool negative){
	pulse2SweepNegative = negative;
}

void Audio::setPulse2SweepShift(unsigned char shift){
	pulse2SweepShift = shift;
}

void Audio::setPulse2TimerHigh(unsigned char high){
	pulse2Timer = (high << 8) + (pulse2Timer & 0x00ff);
}

void Audio::setPulse2TimerLow(unsigned char low){
	pulse2Timer = low + (pulse2Timer & 0xff00);
}

void Audio::setPulse2Length(unsigned char length){
	if (pulse2Enable) pulse2Length = getLength(length);
	pulse2Phase = 0;
	pulse2StartFlag = true;
}

void Audio::setTriangleHalt(bool halt){
	triangleHalt = halt;
}

void Audio::setTriangleFrameCount(unsigned char frame){
	triangleFrameCount = frame;
}

void Audio::setTriangleTimerHigh(unsigned char high){
	triangleTimer = (high << 8) + (triangleTimer & 0x00ff);
}

void Audio::setTriangleTimerLow(unsigned char low){
	triangleTimer = low + (triangleTimer & 0xff00);
}

void Audio::setTriangleLength(unsigned char length){
	if (triangleEnable) triangleLength = getLength(length);
}

void Audio::setNoiseHalt(bool halt){
	noiseHalt = halt;
}

void Audio::setNoiseEnvelope(bool envelope){
	noiseEnvelope = envelope;
}

void Audio::setNoiseEnvelopePeriod(unsigned char period){
	noiseEnvelopePeriod = period;
}

void Audio::setNoiseLoop(bool loop){
	noiseLoop = loop;
}

void Audio::setNoisePeriod(unsigned char period){
	noisePeriod = period;
}

void Audio::setNoiseLength(unsigned char length){
	if (noiseEnable) noiseLength = getLength(length);
}

void Audio::setDeltaInterrupt(bool interrupt){
	deltaInterrupt = interrupt;
}

void Audio::setDeltaLoop(bool loop){
	deltaLoop = loop;
}

void Audio::setDeltaFrequency(unsigned char frequency){
	deltaFrequency = frequency;
}

void Audio::setDeltaDirect(unsigned char direct){
	deltaDirect = direct;
}

void Audio::setDeltaAddress(unsigned char address){
	deltaAddress = address;
}

void Audio::setDeltaLength(unsigned char length){
	deltaLength = length;
}

void Audio::setPulse1Enable(bool enable){
	pulse1Enable = enable;
	if (!enable) pulse1Length = 0;
}

void Audio::setPulse2Enable(bool enable){
	pulse2Enable = enable;
	if (!enable) pulse2Length = 0;
}

void Audio::setTriangleEnable(bool enable){
	triangleEnable = enable;
	if (!enable) triangleLength = 0;
}

void Audio::setNoiseEnable(bool enable){
	noiseEnable = enable;
	if (!enable) noiseLength = 0;
}

void Audio::setDeltaEnable(bool enable){
	deltaEnable = enable;
}

void Audio::setFrameSequence(bool sequence){
	frameSequence = sequence;
	frameCount = 0;
}

void Audio::setFrameInterrupt(bool interrupt){
	std::cout << "setting frame interrupt enable to " << int(interrupt) << "\n";
	frameInterrupt = interrupt;
}

void Audio::setCpu(CPU* cpu){
	cpu = cpu;
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
