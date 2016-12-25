#include "nintendulator.h"

bool debug;

unsigned short programRomPageCount;
unsigned short characterRomPageCount;
unsigned char flags1;
unsigned char flags2;
unsigned short mapper;
unsigned char submapperNumber;
bool fourScreenMode;
bool trainer;
bool batteryBackedRam;
bool mirroring;
bool nes2Mode;
bool playchoice10;
bool vsUnisystem;

unsigned short PC;
unsigned char SP;
unsigned char A;
unsigned char X;
unsigned char Y;
unsigned char P;

std::vector<unsigned char> romBytes;
unsigned char RAM[0x0800];
unsigned char cartRAM[0x2000];
unsigned char programBank0;
unsigned char programBank1;

int main(int argc, char *argv[]){
	debug = false;
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

	programRomPageCount = headerBytes[4];
	characterRomPageCount = headerBytes[5];
	flags1 = headerBytes[6];
	flags2 = headerBytes[7];

	mapper = (flags1 >> 4) + (flags2 & 0xf0);
	submapperNumber = 0;
	fourScreenMode = flags1 & 0x08;
	trainer = flags1 & 0x04;
	batteryBackedRam = flags1 & 0x02;
	mirroring = flags1 & 0x01;
	nes2Mode = ((flags2 & 0x0c) == 0x08);
	playchoice10 = flags2 & 0x02;
	vsUnisystem = flags2 & 0x01;

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

	while (inFile.get(inChar)){
		romBytes.push_back(inChar);
	}
	inFile.close();

	if (debug){
		std::cout<< "program rom pages: " << programRomPageCount << "\n";
		std::cout<< "program rom bytes: " << programRomPageCount*16384 << "\n";
		std::cout<< "character rom pages: " << characterRomPageCount << "\n";
		std::cout<< "character rom bytes: " << characterRomPageCount*16384 << "\n";
		std::cout<< "total rom bytes: " << romBytes.size() << "\n";
		std::cout<< "total file bytes: " << headerBytes.size()+romBytes.size() << "\n";
	}

	PC = 0;
	SP = 0xff;
	A = 0;
	X = 0;
	Y = 0;
	P = 0;

	programBank0 = 0;
	programBank1 = 0;
	if (mapper == 2){
		programBank1 = programRomPageCount-1;
	}

	// reset interrupt on startup
	if (debug) std::cout<< "Reset PC to " << std::hex << (valueAt(0xfffd) << 8) + valueAt(0xfffc) << "\n";
	PC = (valueAt(0xfffd) << 8) + valueAt(0xfffc);

	if (!View::init()){
		std::cout<<"Error constructing View. Exiting";
		return 0;
	}

	std::string dummy;
	unsigned char opcode;
	unsigned char cycles;
	unsigned char nextCPU = 1;
	while (1){
		if (--nextCPU == 0){
			if (debug){
				std::cout<< "*****CPU STATUS*****\n";
				std::cout<< std::hex << "PC: " << int(PC) << ", A: " << int(A) << ", X: " << int(X) << ", Y: " << int(Y) << ", P: " << int(P) << ", SP: " << int(SP) << "\n";
				std::cout<< "NEXT IS: " << int(valueAt(PC)) << " " << int(valueAt(PC+1)) << " " << int(valueAt(PC+2)) << " " << int(valueAt(PC+3)) << "\n";
				std::cin>>dummy;
			}

			opcode = valueAt(PC++);
			cycles = Execute::execute(opcode);
			if (!debug && cycles == 0){
				break;
			}
			nextCPU = cycles;
		}
		if (!View::event()){
			break;
		}
		View::render();
		std::this_thread::sleep_for(std::chrono::microseconds(periodMicroseconds));
	}

	View::destroy();

	return 0;
}

unsigned char valueAt(unsigned short address){
	unsigned char ret = 0;
	if (address > 0xffff){
		std::cout<< "Cannot fetch value at address " << address << ". Invalid address." << "\n";
	} else if (address > 0xBfff){
		ret = romBytes[(programBank1 * 0x400) + (address-0xC000)];
	} else if (address > 0x7fff){
		ret = romBytes[(programBank0 * 0x400) + (address-0x8000)];
	} else if (address > 0x5fff){
		ret = cartRAM[address-0x6000];
	} else if (address > 0x401f){
		// mapper specific space
	} else if (address > 0x3fff){
		// get I/O register
	} else if (address > 0x1fff){
		ret = PPU::getReg((address-0x2000) % 0x008);
	} else{ // address < 0x2000
		ret = RAM[address % 0x0800];
	}
	return ret;
}

void setValueAt(unsigned short address, unsigned char value){
	unsigned char ret = 0;
	if (address > 0xffff){
		std::cout<< "Cannot set value at address " << address << ". Invalid address." << "\n";
	} else if (address > 0xBfff){
		if (mapper == 2){
			// pass
		} else{
			romBytes[(programBank1 * 0x400) + (address-0xC000)] = value;
		}
	} else if (address > 0x7fff){
		if (mapper == 2){
			if (debug) std::cout << "Switching to bank " << int(value & 0x07) << "\n";
			programBank0 = value & 0x07;
		} else{
			romBytes[(programBank0 * 0x400) + (address-0x8000)] = value;
		}
	} else if (address > 0x5fff){
		cartRAM[address-0x6000] = value;
	} else if (address > 0x401f){
		// mapper specific space
	} else if (address > 0x3fff){
		// set I/O register
	} else if (address > 0x1fff){
		PPU::setReg((address-0x2000) % 0x008, value);
	} else{
		RAM[address % 0x0800] = value;
	}
}