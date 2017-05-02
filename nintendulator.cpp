#include "nintendulator.h"

bool debug;
bool test;
bool patternDump;
bool nameDump;

constexpr int cpuPeriodNanoseconds = 559;
constexpr int framePeriodNanoseconds = 16666667;

int main(int argc, char *argv[]){
	debug = false;
	test = false;
	std::string filename;
	if (argc > 1){
		filename = argv[1];
	} else{
		std::cout<<"no rom file provided. Exiting"<<"\n";
		return 0;
	}
	for (int i = 0; i < argc-2; i++){
		if (std::string(argv[i+2]) == "-d"){
			debug = true;
		}
		if (std::string(argv[i+2]) == "-t"){
			test = true;
		}
		if (std::string(argv[i+2]) == "-p"){
			patternDump = true;
		}
		if (std::string(argv[i+2]) == "-n"){
			nameDump = true;
		}
	}

	std::vector<unsigned char> headerBytes;
	char inChar;
	std::ifstream inFile;
	inFile.open("roms/" + filename);
	if (!inFile.is_open()){
		std::cout<<"could not open rom file " << filename << ". Exiting"<<"\n";
		return 0;
	}
	int i = 0;
	while (i < 16 && inFile.get(inChar)){
		headerBytes.push_back(inChar);
		i++;
	}
	if (headerBytes.size() < 16){
		std::cout<<"ERROR: Could not read rom file header. Exiting";
		return 0;
	}

	unsigned short programRomPageCount = headerBytes[4];
	unsigned short characterRomPageCount = headerBytes[5];
	unsigned char flags1 = headerBytes[6];
	unsigned char flags2 = headerBytes[7];

	unsigned short mapper = (flags1 >> 4) + (flags2 & 0xf0);
	unsigned char submapperNumber = 0;
	bool fourScreenMode = flags1 & 0x08;
	bool trainer = flags1 & 0x04;
	bool batteryBackedRam = flags1 & 0x02;
	bool mirroring = flags1 & 0x01;
	bool nes2Mode = ((flags2 & 0x0c) == 0x08);
	bool playchoice10 = flags2 & 0x02;
	bool vsUnisystem = flags2 & 0x01;

	if (nes2Mode){
		mapper += (headerBytes[8] & 0x0f) * 256;
		submapperNumber = headerBytes[8] >> 4;
		programRomPageCount += (headerBytes[9] & 0x0f) * 256;
		characterRomPageCount += (headerBytes[9] >> 4) * 256;
		// TODO: battery backed / not battery backed RAM
		//       NTSC/PAL
		//       vs. Hardware
	} else{
		// TODO: NES1 spec header
	}

	if (mapper != 2 && mapper != 0){
		std::cout<<"Mapper "<< mapper << " has not been implemented. Exiting";
		return 0;
	}

	unsigned char *cartRAM = new unsigned char[0x2000];
	std::vector<unsigned char>* romBytes = new std::vector<unsigned char>();
	while (inFile.get(inChar)){
		romBytes->push_back(inChar);
	}
	inFile.close();

	if (debug || test){
		std::cout<< "program rom pages: " << programRomPageCount << "\n";
		std::cout<< "expected program rom bytes: " << programRomPageCount*16384 << "\n";
		std::cout<< "program rom bytes: " << romBytes->size() << "\n";
		std::cout<< "character rom pages: " << characterRomPageCount << "\n";
		std::cout<< "character rom bytes: " << characterRomPageCount*16384 << "\n";
		std::cout<< "total rom bytes: " << romBytes->size() << "\n";
		std::cout<< "total file bytes: " << headerBytes.size()+romBytes->size() << "\n";
	}

	Controller* player1 = new Controller(true);
	Controller* player2 = new Controller(false);
	PPU* ppu = new PPU(mirroring, fourScreenMode);
	APU* apu = new APU(new Audio());
	CPU* cpu = new CPU(romBytes, cartRAM, programRomPageCount, mapper, ppu, apu, player1, player2);
	if (debug) cpu->setDebug();
	// reset interrupt on startup
	cpu->reset();

	View* view = new View(ppu, player1, player2);
	if (!view->init()){
		std::cout<<"Error constructing View. Exiting";
		return 0;
	}

	std::string dummy;
	auto start = std::chrono::high_resolution_clock::now();
	auto now = std::chrono::high_resolution_clock::now();
	auto prev = std::chrono::high_resolution_clock::now();
	int frames = 0;
	unsigned char opcode;
	unsigned char cycles;
	unsigned char nextCPU = 1;
	int masterClock = 0;
	while (1){
		if (--nextCPU == 0){ // cpu clock cycle
			cycles = cpu->execute();
			// if (!debug && cycles == 0){
			// 	break;
			// }
			nextCPU = cycles;
		}
		if (ppu->stallCPU()) nextCPU += 513; // OAMDMA
		for (int i = 0; i < 3; i++){ // 3 ppu cycles per cpu cycle
			ppu->execute();
			if (ppu->nmi()){
				cpu->nmi();
			}
		}

		if (++masterClock == 29868){ // 1/60 seconds worth of clock cycles
			if (patternDump) ppu->dumpPatternTable();
			else if (nameDump) ppu->dumpNameTable();
			view->render();
			while (view->event()){
				// process all events
			}
			if (view->quit()){
				break;
			}
			frames++;
			// sleep for the remaining time
			bool sleep = true;
			now = std::chrono::high_resolution_clock::now();
			auto elapsed = std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1,1000000000>>>(now - prev);
			while (elapsed.count() < framePeriodNanoseconds){
				now = std::chrono::high_resolution_clock::now();
				elapsed = std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1,1000000000>>>(now - prev);
			}
			prev = now;
			masterClock = 0;
		}
	}
	view->destroy();
	now = std::chrono::high_resolution_clock::now();
	std::cout<< std::dec << frames << " frames in " << (now-start).count() << " ns\n";

	if (test){
		cpu->writeRamToFile("roms/testoutput.txt");
	}

	return 0;
}