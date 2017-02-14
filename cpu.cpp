#include "cpu.h"

unsigned char CPU::valueAt(unsigned short address){
	unsigned char ret = 0;
	if (address > 0xffff){
		std::cout<< "Cannot fetch value at address " << address << ". Invalid address." << "\n";
	} else if (address > 0xBfff){
		ret = (*romBytes)[(programBank1 * 0x4000) + (address-0xC000)];
	} else if (address > 0x7fff){
		ret = (*romBytes)[(programBank0 * 0x4000) + (address-0x8000)];
	} else if (address > 0x5fff){
		ret = cartRAM[address-0x6000];
	} else if (address > 0x401f){
		// mapper specific space
	} else if (address > 0x3fff){
		// get I/O register
	} else if (address > 0x1fff){
		ret = ppu->getReg((address-0x2000) % 0x008);
	} else{ // address < 0x2000
		ret = RAM[address % 0x0800];
	}
	return ret;
}

void CPU::setValueAt(unsigned short address, unsigned char value){
	unsigned char ret = 0;
	if (address > 0xffff){
		std::cout<< "Cannot set value at address " << address << ". Invalid address." << "\n";
	} else if (address > 0xBfff){
		if (mapper == 2){
			if (debug) std::cout << "Switching to bank " << int(value % programBankCount) << "\n";
			programBank0 = value % programBankCount;
		} else{
			(*romBytes)[(programBank1 * 0x4000) + (address-0xC000)] = value;
		}
	} else if (address > 0x7fff){
		if (mapper == 2){
			if (debug) std::cout << "Switching to bank " << int(value % programBankCount) << "\n";
			programBank0 = value % programBankCount;
		} else{
			(*romBytes)[(programBank0 * 0x4000) + (address-0x8000)] = value;
		}
	} else if (address > 0x5fff){
		cartRAM[address-0x6000] = value;
	} else if (address > 0x401f){
		// mapper specific space
	} else if (address > 0x3fff){
		if (address == 0x4014) ppu->oamdma(value, this);
		// set I/O register
	} else if (address > 0x1fff){
		ppu->setReg((address-0x2000) % 0x008, value);
	} else{ // address < 0x2000
		RAM[address % 0x0800] = value;
	}
}

CPU::CPU(std::vector<unsigned char>* romBytes, unsigned char* cartRAM, unsigned short programBankCount, unsigned short mapper, PPU* ppu){
	PC = 0;
	SP = 0xff;
	A = 0;
	X = 0;
	Y = 0;
	P = 0;

	this->romBytes = romBytes;
	this->RAM = new unsigned char[0x0800];
	this->cartRAM = cartRAM;
	this->programBankCount = programBankCount;
	this->mapper = mapper;
	this->programBank0 = 0;
	this->programBank1 = 0;
	if (mapper == 2 || mapper == 0){
		this->programBank1 = programBankCount-1;
	}
	this->debug = false;
	this->ppu = ppu;
}

void CPU::reset(){
	if (debug) std::cout<< "Reset PC to " << std::hex << (valueAt(0xfffd) << 8) + valueAt(0xfffc) << "\n";
	PC = (valueAt(0xfffd) << 8) + valueAt(0xfffc);
}

void CPU::nmi(){
	setValueAt(0x100 + SP--, (PC & 0xff00) >> 8);
	setValueAt(0x100 + SP--, (PC & 0x00ff));
	setValueAt(0x100 + SP--, P);
	PC = (valueAt(0xfffb) << 8) + valueAt(0xfffa);
}

void CPU::setDebug(){
	debug = true;
}

void CPU::writeRamToFile(std::string filename){
	std::ofstream outFile;
	outFile.open(filename, std::ios_base::trunc);
	for (int i = 0; i < 0x800; i++){
		outFile.put(RAM[i]);
	}
	for (int i = 0; i < romBytes->size(); i++){
		outFile.put((*romBytes)[i]);
	}
	outFile.close();
}

/*
* Calls the instruction function corresponding to opcode
* Instruction function names are instruction name followed by addressing mode
* Addressing modes are:
* IM  - immediate
* ZP  - zero page
* ZPX - zero page + X
* ZPY - zero page + Y
* AB  - absolute
* ABX - absolute + X
* ABY - absolute + Y
* IN  - Indirect
* INX - Indirect X
* INY - Indirect Y
* ACC - accumulator
*/
unsigned char CPU::execute(){
	if (debug){
		std::cout<< "*****CPU STATUS*****\n";
		std::cout<< std::hex << "PC: " << int(PC) << ", A: " << int(A) << ", X: " << int(X) << ", Y: " << int(Y) << ", P: " << int(P) << ", SP: " << int(SP) << "\n";
		std::cout<< "NEXT IS: " << int(valueAt(PC)) << " " << int(valueAt(PC+1)) << " " << int(valueAt(PC+2)) << " " << int(valueAt(PC+3)) << "\n";
		// std::cin>>dummy;
		// std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	unsigned char opcode = valueAt(PC++);
	int cycles = 1;
	switch(opcode) {
		// ADC - Add With Carry
		case 0x69: cycles = ADC_IM();
				   break;
		case 0x65: cycles = ADC_ZP();
				   break;
		case 0x75: cycles = ADC_ZPX();
				   break;
		case 0x6d: cycles = ADC_AB();
				   break;
		case 0x7d: cycles = ADC_ABX();
				   break;
		case 0x79: cycles = ADC_ABY();
				   break;
		case 0x61: cycles = ADC_INX();
				   break;
		case 0x71: cycles = ADC_INY();
				   break;

		// ANC - AND then Copy N to C
		case 0x2b: cycles = ANC();
				   break;

		// AND - Logical AND
		case 0x29: cycles = AND_IM();
				   break;
		case 0x25: cycles = AND_ZP();
				   break;
		case 0x35: cycles = AND_ZPX();
				   break;
		case 0x2d: cycles = AND_AB();
				   break;
		case 0x3d: cycles = AND_ABX();
				   break;
		case 0x39: cycles = AND_ABY();
				   break;
		case 0x21: cycles = AND_INX();
				   break;
		case 0x31: cycles = AND_INY();
				   break;

		// ASL - Arithmetic Shift Left
		case 0x0a: cycles = ASL_ACC();
				   break;
		case 0x06: cycles = ASL_ZP();
				   break;
		case 0x16: cycles = ASL_ZPX();
				   break;
		case 0x0e: cycles = ASL_AB();
				   break;
		case 0x1e: cycles = ASL_ABX();
				   break;

		// BCC - Branch if Carry Cleared
		case 0x90: cycles = BCC();
				   break;

		// BCS - Branch if Carry Set
		case 0xb0: cycles = BCS();
				   break;

		// BEQ - Branch if Equal
		case 0xf0: cycles = BEQ();
				   break;

		// BIT - Bit Test
		case 0x24: cycles = BIT_ZP();
				   break;
		case 0x2c: cycles = BIT_AB();
				   break;

		// BMI - Branch if Minus
		case 0x30: cycles = BMI();
				   break;

		// BNE - Branch if Not Equal
		case 0xd0: cycles = BNE();
				   break;

		// BPL - Branch if Positive
		case 0x10: cycles = BPL();
				   break;

		// BRK - Force Interrupt
		case 0x00: cycles = BRK();
				   break;

		// BVC - Branch if Overflow Clear
		case 0x50: cycles = BVC();
				   break;

		// BVS - Branch if Overflow Set
		case 0x70: cycles = BVS();
				   break;

		// CLC - Clear Carry Flag
		case 0x18: cycles = CLC();
				   break;

		// CLD - Clear Decimal Flag
		case 0xd8: cycles = CLD();
				   break;

		// CLI - Clear Interrupt Disable Flag
		case 0x58: cycles = CLI();
				   break;

		// CLV - Clear Overflow Flag
		case 0xb8: cycles = CLV();
				   break;

		// CMP - Compare
		case 0xc9: cycles = CMP_IM();
				   break;
		case 0xc5: cycles = CMP_ZP();
				   break;
		case 0xd5: cycles = CMP_ZPX();
				   break;
		case 0xcd: cycles = CMP_AB();
				   break;
		case 0xdd: cycles = CMP_ABX();
				   break;
		case 0xd9: cycles = CMP_ABY();
				   break;
		case 0xc1: cycles = CMP_INX();
				   break;
		case 0xd1: cycles = CMP_INY();
				   break;

		// CPX - Compare X
		case 0xe0: cycles = CPX_IM();
				   break;
		case 0xe4: cycles = CPX_ZP();
				   break;
		case 0xec: cycles = CPX_AB();
				   break;

		// CPX - Compare Y
		case 0xc0: cycles = CPY_IM();
				   break;
		case 0xc4: cycles = CPY_ZP();
				   break;
		case 0xcc: cycles = CPY_AB();
				   break;

		// DCP - Decrement then Compare
		case 0xc7: cycles = DCP_ZP();
				   break;
		case 0xdf: cycles = DCP_ABX();
				   break;
		case 0xdb: cycles = DCP_ABY();
				   break;

		// DEC - Decrement Memory
		case 0xc6: cycles = DEC_ZP();
				   break;
		case 0xd6: cycles = DEC_ZPX();
				   break;
		case 0xce: cycles = DEC_AB();
				   break;
		case 0xde: cycles = DEC_ABX();
				   break;

		// DEX - Decrement X
		case 0xca: cycles = DEX();
				   break;

		// DEY - Decrement Y
		case 0x88: cycles = DEY();
				   break;

		// EOR - Exclusive Or
		case 0x49: cycles = EOR_IM();
				   break;
		case 0x45: cycles = EOR_ZP();
				   break;
		case 0x55: cycles = EOR_ZPX();
				   break;
		case 0x4d: cycles = EOR_AB();
				   break;
		case 0x5d: cycles = EOR_ABX();
				   break;
		case 0x59: cycles = EOR_ABY();
				   break;
		case 0x41: cycles = EOR_INX();
				   break;
		case 0x51: cycles = EOR_INY();
				   break;

		// IGN - Ignore
		case 0x04: cycles = IGN_ZP();
				   break;
		case 0x14: cycles = IGN_ZPX();
				   break;
		case 0xfc:
		case 0xdc:
		case 0x3c: cycles = IGN_ABX();
				   break;

		// INC - Increment Memory
		case 0xe6: cycles = INC_ZP();
				   break;
		case 0xf6: cycles = INC_ZPX();
				   break;
		case 0xee: cycles = INC_AB();
				   break;
		case 0xfe: cycles = INC_ABX();
				   break;

		// INX - Increment X
		case 0xe8: cycles = INX();
				   break;

		// INY - Increment Y
		case 0xc8: cycles = INY();
				   break;

		// ISC - Increment then Subtract
		case 0xf7: cycles = ISC_ZPX();
				   break;
		case 0xef: cycles = ISC_AB();
				   break;
		case 0xff: cycles = ISC_ABX();
				   break;
		case 0xfb: cycles = ISC_ABY();
				   break;
		case 0xe3: cycles = ISC_INX();
				   break;

		// JMP - Jump
		case 0x4c: cycles = JMP_AB();
				   break;
		case 0x6c: cycles = JMP_IN();
				   break;

		// JSR - Jump to Subroutine
		case 0x20: cycles = JSR();
				   break;

		// KIL - Kill Program
		case 0x02:
		case 0x62: cycles = KIL();
				   break;

		// LAX - Load Accumulator then Transfer to X
		case 0xbf: cycles = LAX_ABY();
				   break;

		// LDA - Load Accumulator
		case 0xa9: cycles = LDA_IM();
				   break;
		case 0xa5: cycles = LDA_ZP();
				   break;
		case 0xb5: cycles = LDA_ZPX();
				   break;
		case 0xad: cycles = LDA_AB();
				   break;
		case 0xbd: cycles = LDA_ABX();
				   break;
		case 0xb9: cycles = LDA_ABY();
				   break;
		case 0xa1: cycles = LDA_INX();
				   break;
		case 0xb1: cycles = LDA_INY();
				   break;

		// LDX - Load X
		case 0xa2: cycles = LDX_IM();
				   break;
		case 0xa6: cycles = LDX_ZP();
				   break;
		case 0xb6: cycles = LDX_ZPY();
				   break;
		case 0xae: cycles = LDX_AB();
				   break;
		case 0xbe: cycles = LDX_ABY();
				   break;

		// LDY - Load Y
		case 0xa0: cycles = LDY_IM();
				   break;
		case 0xa4: cycles = LDY_ZP();
				   break;
		case 0xb4: cycles = LDY_ZPX();
				   break;
		case 0xac: cycles = LDY_AB();
				   break;
		case 0xbc: cycles = LDY_ABX();
				   break;

		// LSR - Logical Shift Right
		case 0x4a: cycles = LSR_ACC();
				   break;
		case 0x46: cycles = LSR_ZP();
				   break;
		case 0x56: cycles = LSR_ZPX();
				   break;
		case 0x4e: cycles = LSR_AB();
				   break;
		case 0x5e: cycles = LSR_ABX();
				   break;

		// NOP - No Operation
		case 0xea: cycles = NOP();
				   break;

		// ORA - Logical OR
		case 0x09: cycles = ORA_IM();
				   break;
		case 0x05: cycles = ORA_ZP();
				   break;
		case 0x15: cycles = ORA_ZPX();
				   break;
		case 0x0d: cycles = ORA_AB();
				   break;
		case 0x1d: cycles = ORA_ABX();
				   break;
		case 0x19: cycles = ORA_ABY();
				   break;
		case 0x01: cycles = ORA_INX();
				   break;
		case 0x11: cycles = ORA_INY();
				   break;

		// PHA - Push Accumulator
		case 0x48: cycles = PHA();
				   break;

		// PHP - Push Processor Status
		case 0x08: cycles = PHP();
				   break;

		// PLA - Pull Accumulator
		case 0x68: cycles = PLA();
				   break;

		// PLP - Pull Processopr Status
		case 0x28: cycles = PLP();
				   break;

		// RLA - Rotate Left the AND
		case 0x3f: cycles = RLA_ABX();
				   break;

		// ROL - Rotate Left
		case 0x2a: cycles = ROL_ACC();
				   break;
		case 0x26: cycles = ROL_ZP();
				   break;
		case 0x36: cycles = ROL_ZPX();
				   break;
		case 0x2e: cycles = ROL_AB();
				   break;
		case 0x3e: cycles = ROL_ABX();
				   break;

		// ROR - Rotate Right
		case 0x6a: cycles = ROR_ACC();
				   break;
		case 0x66: cycles = ROR_ZP();
				   break;
		case 0x76: cycles = ROR_ZPX();
				   break;
		case 0x6e: cycles = ROR_AB();
				   break;
		case 0x7e: cycles = ROR_ABX();
				   break;

		// RTI - Return from Interrupt
		case 0x40: cycles = RTI();
				   break;

		// RTS - Return from Subroutine
		case 0x60: cycles = RTS();
				   break;

		// SAX - Store A AND X
		case 0x87: cycles = SAX_ZP();
				   break;
		case 0x83: cycles = SAX_INX();
				   break;

		// SBC - Subtract With Carry
		case 0xe9: cycles = SBC_IM();
				   break;
		case 0xe5: cycles = SBC_ZP();
				   break;
		case 0xf5: cycles = SBC_ZPX();
				   break;
		case 0xed: cycles = SBC_AB();
				   break;
		case 0xfd: cycles = SBC_ABX();
				   break;
		case 0xf9: cycles = SBC_ABY();
				   break;
		case 0xe1: cycles = SBC_INX();
				   break;
		case 0xf1: cycles = SBC_INY();
				   break;

		// SEC - Set Carry Flag
		case 0x38: cycles = SEC();
				   break;

		// SED - Set Decimal Flag
		case 0xf8: cycles = SED();
				   break;

		// SEI - Set Interrupt Disable Flag
		case 0x78: cycles = SEI();
				   break;

		// SKB - Skip Byte
		case 0x80:
		case 0x82: cycles = SKB();
				   break;

		// SLO - Shift Left then OR
		case 0x17: cycles = SLO_ZPX();
				   break;
		case 0x0f: cycles = SLO_AB();
				   break;
		case 0x1f: cycles = SLO_ABX();
				   break;
		case 0x03: cycles = SLO_INX();
				   break;

		// SRE - Shift Right then EOR
		case 0x4f: cycles = SRE_AB();
				   break;
		case 0x53: cycles = SRE_INY();
				   break;

		// STA - Store Accumulator
		case 0x85: cycles = STA_ZP();
				   break;
		case 0x95: cycles = STA_ZPX();
				   break;
		case 0x8d: cycles = STA_AB();
				   break;
		case 0x9d: cycles = STA_ABX();
				   break;
		case 0x99: cycles = STA_ABY();
				   break;
		case 0x81: cycles = STA_INX();
				   break;
		case 0x91: cycles = STA_INY();
				   break;

		// STX - Store X
		case 0x86: cycles = STX_ZP();
				   break;
		case 0x96: cycles = STX_ZPY();
				   break;
		case 0x8e: cycles = STX_AB();
				   break;

		// STY - Store Y
		case 0x84: cycles = STY_ZP();
				   break;
		case 0x94: cycles = STY_ZPX();
				   break;
		case 0x8c: cycles = STY_AB();
				   break;

		// TAX - Transfer Accumulator to X
		case 0xaa: cycles = TAX();
				   break;

		// TAY - Transfer Accumulator to Y
		case 0xa8: cycles = TAY();
				   break;

		// TSX - Transfer Stack Pointer to X
		case 0xba: cycles = TSX();
				   break;

		// TXA - Transfer X to Accumulator
		case 0x8a: cycles = TXA();
				   break;

		// TXS - Transfer X to Stack Pointer
		case 0x9a: cycles = TXS();
				   break;

		// TYA - Transfer Y to Accumulator
		case 0x98: cycles = TYA();
				   break;

		default:   std::cout << "Opcode " << std::hex << int(opcode) << " not recognized." << "\n";
				   break;
	}
	return cycles;
}

/******************************************************************************
* Operand address fetching
******************************************************************************/
unsigned short CPU::_IM(){
	// if (debug) std::cout << "fetching value " << int(valueAt(PC)) << " from address " << int(PC) << "\n";
	return PC++;
}
unsigned short CPU::_ZP(){
	// if (debug) std::cout << "fetching value " << int(valueAt(valueAt(PC))) << " from address " << int(valueAt(PC)) << "\n";
	return valueAt(PC++);
}
unsigned short CPU::_ZPX(){
	// if (debug) std::cout << "fetching value " << int(valueAt(valueAt(PC) + X % 0x100)) << " from address " << int(valueAt(PC) + X % 0x100) << "\n";
	return (valueAt(PC++) + X) % 0x100;
}
unsigned short CPU::_ZPY(){
	// if (debug) std::cout << "fetching value " << int(valueAt(valueAt(PC) + Y % 0x100)) << " from address " << int(valueAt(PC) + Y % 0x100) << "\n";
	return (valueAt(PC++) + Y) % 0x100;
}
unsigned short CPU::_AB(){
	unsigned short ret = valueAt(PC) + (valueAt(PC+1) << 8);
	// if (debug) std::cout << "fetching value " << int(valueAt(ret)) << " from address " << int(ret) << "\n";
	PC += 2;
	return ret;
}
unsigned short CPU::_ABX(){
	unsigned short ret = valueAt(PC) + (valueAt(PC+1) << 8) + X;
	// if (debug) std::cout << "fetching value " << int(valueAt(ret)) << " from address " << int(ret) << "\n";
	PC += 2;
	return ret;
}
unsigned short CPU::_ABY(){
	unsigned short ret = valueAt(PC) + (valueAt(PC+1) << 8) + Y;
	// if (debug) std::cout << "fetching value " << int(valueAt(ret)) << " from address " << int(ret) << "\n";
	PC += 2;
	return ret;
}
unsigned short CPU::_INX(){
	unsigned short ret = ((valueAt(PC++) + X) % 0x100);
	ret = valueAt(ret) + (valueAt((ret+1) % 0x100) << 8);
	// if (debug) std::cout << "fetching value " << int(valueAt(ret)) << " from address " << int(ret) << "\n";
	return ret;
}
unsigned short CPU::_INY(){
	unsigned short ret = valueAt(valueAt(PC)) + (valueAt(valueAt(PC)+1) << 8) + Y;
	// if (debug) std::cout << "fetching value " << int(valueAt(ret)) << " from address " << int(ret) << "\n";
	PC++;
	return ret;
}

/******************************************************************************
* ADC - Add with Carry
******************************************************************************/
unsigned char CPU::ADC_IM(){
	if (debug) std::cout<< "ADC_IM: ";
	ADC(valueAt(_IM()));
	return 2;
}
unsigned char CPU::ADC_ZP(){
	if (debug) std::cout<< "ADC_ZP: ";
	ADC(valueAt(_ZP()));
	return 3;
}
unsigned char CPU::ADC_ZPX(){
	if (debug) std::cout<< "ADC_ZPX: ";
	ADC(valueAt(_ZPX()));
	return 4;
}
unsigned char CPU::ADC_AB(){
	if (debug) std::cout<< "ADC_AB: ";
	ADC(valueAt(_AB()));
	return 4;
}
unsigned char CPU::ADC_ABX(){
	if (debug) std::cout<< "ADC_ABX: ";
	unsigned short addr = _ABX();
	ADC(valueAt(addr));
	if ((addr % 0x100) != (addr-X % 0x100)) return 5;
	else return 4;
}
unsigned char CPU::ADC_ABY(){
	if (debug) std::cout<< "ADC_ABY: ";
	unsigned short addr = _ABY();
	ADC(valueAt(addr));
	if ((addr % 0x100) != (addr-Y % 0x100)) return 5;
	else return 4;
}
unsigned char CPU::ADC_INX(){
	if (debug) std::cout<< "ADC_INX: ";
	ADC(valueAt(_INX()));
	return 6;
}
unsigned char CPU::ADC_INY(){
	if (debug) std::cout<< "ADC_INY: ";
	unsigned short addr = _INY();
	ADC(valueAt(addr));
	if ((addr % 0x100) != (addr-Y % 0x100)) return 6;
	else return 5;
}
void CPU::ADC(unsigned char operand){
	char op1 = char(A);
	char op2 = char(operand);
	char result = op1 + op2 + (P & carryMask);

	if (result == 0) P |= zeroMask; else P &= (0xff - zeroMask);
	if ((result & 0x80) == 0x80) P |= negativeMask; else P &= (0xff - negativeMask);
	if (op1 > 0 && op2 > 0 && (result & 0x80) == 0x80) P |= overflowMask;
	else if (op1 < 0 && op2 < 0 && (result & 0x80) == 0x00) P |= overflowMask;
	else P &= (0xff - overflowMask);
	if (((unsigned short)(op1) + (unsigned short)(op2) & 0x100) == 0x100) P |= carryMask;
	else P &= (0xff - carryMask);
	A = (unsigned char)result;
}

/******************************************************************************
* ANC - AND then Copy N to C
******************************************************************************/
unsigned char CPU::ANC(){
	if (debug) std::cout<< "ANC: ";
	unsigned char operand = valueAt(_IM());

	unsigned char op1 = A;
	unsigned char op2 = operand;
	unsigned char result = op1 & op2;

	if (result == 0) P |= zeroMask; else P &= (0xff - zeroMask);
	if ((result & 0x80) == 0x80) {P |= negativeMask; P |= carryMask;} else {P &= (0xff - negativeMask); P &= (0xff - carryMask);}
	A = result;

	return 2;
}

/******************************************************************************
* AND - Logical AND
******************************************************************************/
unsigned char CPU::AND_IM(){
	if (debug) std::cout<< "AND_IM: ";
	AND(valueAt(_IM()));
	return 2;
}
unsigned char CPU::AND_ZP(){
	if (debug) std::cout<< "AND_ZP: ";
	AND(valueAt(_ZP()));
	return 3;
}
unsigned char CPU::AND_ZPX(){
	if (debug) std::cout<< "AND_ZPX: ";
	AND(valueAt(_ZPX()));
	return 4;
}
unsigned char CPU::AND_AB(){
	if (debug) std::cout<< "AND_AB: ";
	AND(valueAt(_AB()));
	return 4;
}
unsigned char CPU::AND_ABX(){
	if (debug) std::cout<< "AND_ABX: ";
	unsigned short addr = _ABX();
	AND(valueAt(addr));
	if ((addr % 0x100) != (addr-X % 0x100)) return 5;
	else return 4;
}
unsigned char CPU::AND_ABY(){
	if (debug) std::cout<< "AND_ABY: ";
	unsigned short addr = _ABY();
	AND(valueAt(addr));
	if ((addr % 0x100) != (addr-Y % 0x100)) return 5;
	else return 4;
}
unsigned char CPU::AND_INX(){
	if (debug) std::cout<< "AND_INX: ";
	AND(valueAt(_INX()));
	return 6;
}
unsigned char CPU::AND_INY(){
	if (debug) std::cout<< "AND_INY: ";
	unsigned short addr = _INY();
	AND(valueAt(addr));
	if ((addr % 0x100) != (addr-Y % 0x100)) return 6;
	else return 5;
}
void CPU::AND(unsigned char operand){
	unsigned char op1 = A;
	unsigned char op2 = operand;
	unsigned char result = op1 & op2;

	if (result == 0) P |= zeroMask; else P &= (0xff - zeroMask);
	if ((result & 0x80) == 0x80) P |= negativeMask; else P &= (0xff - negativeMask);
	A = result;
}

/******************************************************************************
* ASL - Arithmetic Shift Left
******************************************************************************/
unsigned char CPU::ASL_ACC(){
	if (debug) std::cout<< "ASL_ACC: \n";
	A = ASL(A);
	return 2;
}
unsigned char CPU::ASL_ZP(){
	if (debug) std::cout<< "ASL_ZP: ";
	unsigned short addr = _ZP();
	setValueAt(addr, ASL(valueAt(addr)));
	return 5;
}
unsigned char CPU::ASL_ZPX(){
	if (debug) std::cout<< "ASL_ZPX: ";
	unsigned short addr = _ZPX();\
	setValueAt(addr, ASL(valueAt(addr)));\
	return 6;
}
unsigned char CPU::ASL_AB(){
	if (debug) std::cout<< "ASL_AB: ";
	unsigned short addr = _AB();
	setValueAt(addr, ASL(valueAt(addr)));
	return 6;
}
unsigned char CPU::ASL_ABX(){
	if (debug) std::cout<< "ASL_ABX: ";
	unsigned short addr = _ABX();
	setValueAt(addr, ASL(valueAt(addr)));
	return 7;
}
unsigned char CPU::ASL(unsigned char operand){
	unsigned char result = operand << 1;

	if ((operand & 0x80) == 0x80) P |= carryMask; else P &= (0xff - carryMask);
	if (result == 0) P |= zeroMask; else P &= (0xff - zeroMask);
	if ((result & 0x80) == 0x80) P |= negativeMask; else P &= (0xff - negativeMask);
	return result;
}

/******************************************************************************
* BCC - Branch if Carry Cleared
******************************************************************************/
unsigned char CPU::BCC(){
	if (debug) std::cout<< "BCC: branch to " << int(valueAt(PC)) << "\n";
	char offset = valueAt(PC++);
	unsigned char cycles = 2;
	if ((P & carryMask) == 0x00){
		cycles += 1;
		if ((PC % 0x100) + offset > 0xff) cycles += 2;
		PC += offset;
	}
	return cycles;
}

/******************************************************************************
* BCS - Branch if Carry Set
******************************************************************************/
unsigned char CPU::BCS(){
	if (debug) std::cout<< "BCS: branch to " << int(valueAt(PC)) << "\n";
	char offset = valueAt(PC++);
	unsigned char cycles = 2;
	if ((P & carryMask) == carryMask){
		cycles += 1;
		if ((PC % 0x100) + offset > 0xff) cycles += 2;
		PC += offset;
	}
	return cycles;
}

/******************************************************************************
* BEQ - Branch if Equal
******************************************************************************/
unsigned char CPU::BEQ(){
	if (debug) std::cout<< "BEQ: branch to " << int(valueAt(PC)) << "\n";
	char offset = valueAt(PC++);
	unsigned char cycles = 2;
	if ((P & zeroMask) == zeroMask){
		cycles += 1;
		if ((PC % 0x100) + offset > 0xff) cycles += 2;
		PC += offset;
	}
	return cycles;
}

/******************************************************************************
* BIT - Bit Test
******************************************************************************/
unsigned char CPU::BIT_ZP(){
	if (debug) std::cout<< "BIT_ZP: Bit test " << int(valueAt(valueAt(PC))) << "\n";
	BIT(valueAt(_ZP()));
	return 3;
}
unsigned char CPU::BIT_AB(){
	if (debug) std::cout<< "BIT_AB: Bit test " << int(valueAt(valueAt(PC) + (valueAt(PC+1) << 8))) << "\n";
	BIT(valueAt(_AB()));
	return 4;
}
void CPU::BIT(unsigned char operand){
	if ((A & operand) == 0x00) P |= zeroMask; else P &= (0xff - zeroMask);
	if ((operand & 0x80) == 0x80) P |= negativeMask; else P &= (0xff - negativeMask);
	if ((operand & 0x40) == 0x40) P |= overflowMask; else P &= (0xff - overflowMask);
}

/******************************************************************************
* BMI - Branch if Minus
******************************************************************************/
unsigned char CPU::BMI(){
	if (debug) std::cout<< "BMI: branch to " << int(valueAt(PC)) << "\n";
	char offset = valueAt(PC++);
	unsigned char cycles = 2;
	if ((P & negativeMask) == negativeMask){
		cycles += 1;
		if ((PC % 0x100) + offset > 0xff) cycles += 2;
		PC += offset;
	}
	return cycles;
}

/******************************************************************************
* BNE - Branch if Not Equal
******************************************************************************/
unsigned char CPU::BNE(){
	if (debug) std::cout<< "BNE: branch to " << int(valueAt(PC)) << "\n";
	char offset = valueAt(PC++);
	unsigned char cycles = 2;
	if ((P & zeroMask) == 0x00){
		cycles += 1;
		if ((PC % 0x100) + offset > 0xff) cycles += 2;
		PC += offset;
	}
	return cycles;
}

/******************************************************************************
* BPL - Branch if Positive
******************************************************************************/
unsigned char CPU::BPL(){
	if (debug) std::cout<< "BPL: branch to " << int(valueAt(PC)) << "\n";
	char offset = valueAt(PC++);
	unsigned char cycles = 2;
	if ((P & negativeMask) == 0x00){
		cycles += 1;
		if ((PC % 0x100) + offset > 0xff) cycles += 2;
		PC += offset;
	}
	return cycles;
}

/******************************************************************************
* BRK - Force Interrupt
******************************************************************************/
unsigned char CPU::BRK(){
	if (debug) std::cout<< "BRK: " << "\n";
	if ((P & interruptDisableMask) == 0x00){
		setValueAt(0x100 + SP--, (PC & 0xff00) >> 8);
		setValueAt(0x100 + SP--, (PC & 0x00ff));
		setValueAt(0x100 + SP--, P);
		P |= interruptDisableMask;
		P |= breakMask;
		PC = valueAt(0xfffe) + (valueAt(0xffff) << 8);
	}
	return 7;
}

/******************************************************************************
* BVC - Branch if Overflow Cleared
******************************************************************************/
unsigned char CPU::BVC(){
	if (debug) std::cout<< "BVC: branch to " << int(valueAt(PC)) << "\n";
	char offset = valueAt(PC++);
	unsigned char cycles = 2;
	if ((P & overflowMask) == 0x00){
		cycles += 1;
		if ((PC % 0x100) + offset > 0xff) cycles += 2;
		PC += offset;
	}
	return cycles;
}

/******************************************************************************
* BVS - Branch if Overflow Set
******************************************************************************/
unsigned char CPU::BVS(){
	if (debug) std::cout<< "BVS: branch to " << int(valueAt(PC)) << "\n";
	char offset = valueAt(PC++);
	unsigned char cycles = 2;
	if ((P & overflowMask) == overflowMask){
		cycles += 1;
		if ((PC % 0x100) + offset > 0xff) cycles += 2;
		PC += offset;
	}
	return cycles;
}

/******************************************************************************
* CLC - Clear Carry Flag
******************************************************************************/
unsigned char CPU::CLC(){
	if (debug) std::cout<< "CLC: " << "\n";
	P &= (0xff - carryMask);
	return 2;
}

/******************************************************************************
* CLD - Clear Decimal Flag
******************************************************************************/
unsigned char CPU::CLD(){
	if (debug) std::cout<< "CLC: " << "\n";
	P &= (0xff - decimalMask);
	return 2;
}

/******************************************************************************
* CLI - Clear Interrupt Disable Flag
******************************************************************************/
unsigned char CPU::CLI(){
	if (debug) std::cout<< "CLC: " << "\n";
	P &= (0xff - interruptDisableMask);
	return 2;
}

/******************************************************************************
* CLV - Clear Overflow Flag
******************************************************************************/
unsigned char CPU::CLV(){
	if (debug) std::cout<< "CLC: " << "\n";
	P &= (0xff - overflowMask);
	return 2;
}

/******************************************************************************
* CMP - Compare
******************************************************************************/
unsigned char CPU::CMP_IM(){
	if (debug) std::cout<< "CMP_IM: ";
	CMP(valueAt(_IM()));
	return 2;
}
unsigned char CPU::CMP_ZP(){
	if (debug) std::cout<< "CMP_ZP: ";
	CMP(valueAt(_ZP()));
	return 3;
}
unsigned char CPU::CMP_ZPX(){
	if (debug) std::cout<< "CMP_ZPX: ";
	CMP(valueAt(_ZPX()));
	return 4;
}
unsigned char CPU::CMP_AB(){
	if (debug) std::cout<< "CMP_AB: ";
	CMP(valueAt(_AB()));
	return 4;
}
unsigned char CPU::CMP_ABX(){
	if (debug) std::cout<< "CMP_ABX: ";
	unsigned short addr = _ABX();
	CMP(valueAt(addr));
	if ((addr % 0x100) != (addr-X % 0x100)) return 5;
	else return 4;
}
unsigned char CPU::CMP_ABY(){
	if (debug) std::cout<< "CMP_ABY: ";
	unsigned short addr = _ABY();
	CMP(valueAt(addr));
	if ((addr % 0x100) != (addr-Y % 0x100)) return 5;
	else return 4;
}
unsigned char CPU::CMP_INX(){
	if (debug) std::cout<< "CMP_INX: ";
	CMP(valueAt(_INX()));
	return 6;
}
unsigned char CPU::CMP_INY(){
	if (debug) std::cout<< "CMP_INY: ";
	unsigned short addr = _INY();
	CMP(valueAt(addr));
	if ((addr % 0x100) != (addr-Y % 0x100)) return 6;
	else return 5;
}
void CPU::CMP(unsigned char operand){
	unsigned char op1 = A;
	unsigned char op2 = operand;
	if (op1 >= op2) P |= carryMask; else P &= (0xff - carryMask);
	if (op1 == op2) P |= zeroMask; else P &= (0xff - zeroMask);
	if (op1 < op2) P |= negativeMask; else P &= (0xff - negativeMask);
}

/******************************************************************************
* CPX - Compare X
******************************************************************************/
unsigned char CPU::CPX_IM(){
	if (debug) std::cout<< "CPX_IM: ";
	CPX(valueAt(_IM()));
	return 2;
}
unsigned char CPU::CPX_ZP(){
	if (debug) std::cout<< "CPX_ZP: ";
	CPX(valueAt(_ZP()));
	return 3;
}
unsigned char CPU::CPX_AB(){
	if (debug) std::cout<< "CPX_AB: ";
	CPX(valueAt(_AB()));
	return 4;
}
void CPU::CPX(unsigned char operand){
	unsigned char op1 = X;
	unsigned char op2 = operand;
	if (op1 >= op2) P |= carryMask; else P &= (0xff - carryMask);
	if (op1 == op2) P |= zeroMask; else P &= (0xff - zeroMask);
	if (op1 < op2) P |= negativeMask; else P &= (0xff - negativeMask);
}

/******************************************************************************
* CPY - Compare Y
******************************************************************************/
unsigned char CPU::CPY_IM(){
	if (debug) std::cout<< "CPY_IM: ";
	CPY(valueAt(_IM()));
	return 2;
}
unsigned char CPU::CPY_ZP(){
	if (debug) std::cout<< "CPY_ZP: ";
	CPY(valueAt(_ZP()));
	return 3;
}
unsigned char CPU::CPY_AB(){
	if (debug) std::cout<< "CPY_AB: ";
	CPY(valueAt(_AB()));
	return 4;
}
void CPU::CPY(unsigned char operand){
	unsigned char op1 = Y;
	unsigned char op2 = operand;
	if (op1 >= op2) P |= carryMask; else P &= (0xff - carryMask);
	if (op1 == op2) P |= zeroMask; else P &= (0xff - zeroMask);
	if (op1 < op2) P |= negativeMask; else P &= (0xff - negativeMask);
}

/******************************************************************************
* DCP - Decrement then Compare
******************************************************************************/
unsigned char CPU::DCP_ZP(){
	if (debug) std::cout<< "DCP_ZP: ";
	unsigned short addr = _ZP();
	setValueAt(addr, DCP(valueAt(addr)));
	return 5;
}
unsigned char CPU::DCP_ABX(){
	if (debug) std::cout<< "DCP_ABX: ";
	unsigned short addr = _ABX();
	setValueAt(addr, DCP(valueAt(addr)));
	return 7;
}
unsigned char CPU::DCP_ABY(){
	if (debug) std::cout<< "DCP_ABY: ";
	unsigned short addr = _ABY();
	setValueAt(addr, DCP(valueAt(addr)));
	return 7;
}
unsigned char CPU::DCP(unsigned char operand){
	unsigned char result = operand - 1;

	if (result == 0) P |= zeroMask; else P &= (0xff - zeroMask);
	if ((result & 0x80) == 0x80) P |= negativeMask; else P &= (0xff - negativeMask);

	unsigned char op1 = A;
	unsigned char op2 = result;
	if (op1 >= op2) P |= carryMask; else P &= (0xff - carryMask);
	if (op1 == op2) P |= zeroMask; else P &= (0xff - zeroMask);
	if (op1 < op2) P |= negativeMask; else P &= (0xff - negativeMask);
	return result;
}

/******************************************************************************
* DEC - Decrement
******************************************************************************/
unsigned char CPU::DEC_ZP(){
	if (debug) std::cout<< "DEC_ZP: ";
	unsigned short addr = _ZP();
	setValueAt(addr, DEC(valueAt(addr)));
	return 5;
}
unsigned char CPU::DEC_ZPX(){
	if (debug) std::cout<< "DEC_ZPX: ";
	unsigned short addr = _ZPX();
	setValueAt(addr, DEC(valueAt(addr)));
	return 6;
}
unsigned char CPU::DEC_AB(){
	if (debug) std::cout<< "DEC_AB: ";
	unsigned short addr = _AB();
	setValueAt(addr, DEC(valueAt(addr)));
	return 6;
}
unsigned char CPU::DEC_ABX(){
	if (debug) std::cout<< "DEC_ABX: ";
	unsigned short addr = _ABX();
	setValueAt(addr, DEC(valueAt(addr)));
	return 7;
}
unsigned char CPU::DEC(unsigned char operand){
	unsigned char result = operand - 1;

	if (result == 0) P |= zeroMask; else P &= (0xff - zeroMask);
	if ((result & 0x80) == 0x80) P |= negativeMask; else P &= (0xff - negativeMask);
	return result;
}

/******************************************************************************
* DEX - Decrement X
******************************************************************************/
unsigned char CPU::DEX(){
	if (debug) std::cout<< "DEX: ";
	unsigned char result = X - 1;

	if (result == 0) P |= zeroMask; else P &= (0xff - zeroMask);
	if ((result & 0x80) == 0x80) P |= negativeMask; else P &= (0xff - negativeMask);
	X = result;
	return 2;
}

/******************************************************************************
* DEY - Decrement Y
******************************************************************************/
unsigned char CPU::DEY(){
	if (debug) std::cout<< "DEY: ";
	unsigned char result = Y - 1;

	if (result == 0) P |= zeroMask; else P &= (0xff - zeroMask);
	if ((result & 0x80) == 0x80) P |= negativeMask; else P &= (0xff - negativeMask);
	Y = result;
	return 2;
}

/******************************************************************************
* EOR - Exclusive Or
******************************************************************************/
unsigned char CPU::EOR_IM(){
	if (debug) std::cout<< "EOR_IM: ";
	EOR(valueAt(_IM()));
	return 2;
}
unsigned char CPU::EOR_ZP(){
	if (debug) std::cout<< "EOR_ZP: ";
	EOR(valueAt(_ZP()));
	return 3;
}
unsigned char CPU::EOR_ZPX(){
	if (debug) std::cout<< "EOR_ZPX: ";
	EOR(valueAt(_ZPX()));
	return 4;
}
unsigned char CPU::EOR_AB(){
	if (debug) std::cout<< "EOR_AB: ";
	EOR(valueAt(_AB()));
	return 4;
}
unsigned char CPU::EOR_ABX(){
	if (debug) std::cout<< "EOR_ABX: ";
	unsigned short addr = _ABX();
	EOR(valueAt(addr));
	if ((addr % 0x100) != (addr-X % 0x100)) return 5;
	else return 4;
}
unsigned char CPU::EOR_ABY(){
	if (debug) std::cout<< "EOR_ABY: ";
	unsigned short addr = _ABY();
	EOR(valueAt(addr));
	if ((addr % 0x100) != (addr-Y % 0x100)) return 5;
	else return 4;
}
unsigned char CPU::EOR_INX(){
	if (debug) std::cout<< "EOR_INX: ";
	EOR(valueAt(_INX()));
	return 6;
}
unsigned char CPU::EOR_INY(){
	if (debug) std::cout<< "EOR_INY: ";
	unsigned short addr = _INY();
	EOR(valueAt(addr));
	if ((addr % 0x100) != (addr-Y % 0x100)) return 6;
	else return 5;
}
void CPU::EOR(unsigned char operand){
	unsigned char op1 = A;
	unsigned char op2 = operand;
	unsigned char result = op1 ^ op2;

	if (result == 0) P |= zeroMask; else P &= (0xff - zeroMask);
	if ((result & 0x80) == 0x80) P |= negativeMask; else P &= (0xff - negativeMask);
	A = result;
}

/******************************************************************************
* IGN - Ignore
******************************************************************************/
unsigned char CPU::IGN_ZP(){
	if (debug) std::cout<< "IGN_ZP: ";
	unsigned short addr = _ZP();
	IGN(addr);
	return 3;
}
unsigned char CPU::IGN_ZPX(){
	if (debug) std::cout<< "IGN_ZPX: ";
	unsigned short addr = _ZPX();
	IGN(addr);
	return 3;
}
unsigned char CPU::IGN_ABX(){
	if (debug) std::cout<< "IGN_ABX: ";
	unsigned short addr = _ABX();
	IGN(addr);
	if ((addr % 0x100) != (addr-X % 0x100)) return 5;
	else return 4;
}
void CPU::IGN(unsigned short operand){
	valueAt(operand);
}

/******************************************************************************
* INC - Increment
******************************************************************************/
unsigned char CPU::INC_ZP(){
	if (debug) std::cout<< "INC_ZP: ";
	unsigned short addr = _ZP();
	setValueAt(addr, INC(valueAt(addr)));
	return 5;
}
unsigned char CPU::INC_ZPX(){
	if (debug) std::cout<< "INC_ZPX: ";
	unsigned short addr = _ZPX();
	setValueAt(addr, INC(valueAt(addr)));
	return 6;
}
unsigned char CPU::INC_AB(){
	if (debug) std::cout<< "INC_AB: ";
	unsigned short addr = _AB();
	setValueAt(addr, INC(valueAt(addr)));
	return 6;
}
unsigned char CPU::INC_ABX(){
	if (debug) std::cout<< "INC_ABX: ";
	unsigned short addr = _ABX();
	setValueAt(addr, INC(valueAt(addr)));
	return 7;
}
unsigned char CPU::INC(unsigned char operand){
	unsigned char incResult = operand + 1;

	if (incResult == 0) P |= zeroMask; else P &= (0xff - zeroMask);
	if ((incResult & 0x80) == 0x80) P |= negativeMask; else P &= (0xff - negativeMask);
	return incResult;
}

/******************************************************************************
* INX - Increment X
******************************************************************************/
unsigned char CPU::INX(){
	if (debug) std::cout<< "INX: ";
	unsigned char result = X + 1;

	if (result == 0) P |= zeroMask; else P &= (0xff - zeroMask);
	if ((result & 0x80) == 0x80) P |= negativeMask; else P &= (0xff - negativeMask);
	X = result;
	return 2;
}

/******************************************************************************
* INY - Increment Y
******************************************************************************/
unsigned char CPU::INY(){
	if (debug) std::cout<< "INY: ";
	unsigned char result = Y + 1;

	if (result == 0) P |= zeroMask; else P &= (0xff - zeroMask);
	if ((result & 0x80) == 0x80) P |= negativeMask; else P &= (0xff - negativeMask);
	Y = result;
	return 2;
}

/******************************************************************************
* ISC - Increment then Subtract
******************************************************************************/
unsigned char CPU::ISC_ZPX(){
	if (debug) std::cout<< "ISC_ZPX: ";
	unsigned short addr = _ZPX();
	setValueAt(addr, ISC(valueAt(addr)));
	return 6;
}
unsigned char CPU::ISC_AB(){
	if (debug) std::cout<< "ISC_AB: ";
	unsigned short addr = _AB();
	setValueAt(addr, ISC(valueAt(addr)));
	return 6;
}
unsigned char CPU::ISC_ABX(){
	if (debug) std::cout<< "ISC_ABX: ";
	unsigned short addr = _ABX();
	setValueAt(addr, ISC(valueAt(addr)));
	return 7;
}
unsigned char CPU::ISC_ABY(){
	if (debug) std::cout<< "ISC_ABY: ";
	unsigned short addr = _ABY();
	setValueAt(addr, ISC(valueAt(addr)));
	return 7;
}
unsigned char CPU::ISC_INX(){
	if (debug) std::cout<< "ISC_INX: ";
	unsigned short addr = _INX();
	setValueAt(addr, ISC(addr));
	return 8;
}
unsigned char CPU::ISC(unsigned char operand){
	unsigned char incResult = operand + 1;

	if (incResult == 0) P |= zeroMask; else P &= (0xff - zeroMask);
	if ((incResult & 0x80) == 0x80) P |= negativeMask; else P &= (0xff - negativeMask);

	char op1 = char(A);
	char op2 = char(incResult);
	char result = op1 - op2 + (P & carryMask) - 1;

	if (result == 0) P |= zeroMask; else P &= (0xff - zeroMask);
	if ((result & 0x80) == 0x80) P |= negativeMask; else P &= (0xff - negativeMask);
	if (op1 > 0 && op2 > 0 && (result & 0x80) == 0x80) P |= overflowMask;
	else if (op1 < 0 && op2 < 0 && (result & 0x80) == 0x00) P |= overflowMask;
	else P &= (0xff - overflowMask);
	if (((unsigned short)(op1) + (unsigned short)(op2) & 0x100) == 0x100) P &= (0xff - carryMask);
	else P |= carryMask;
	A = (unsigned char)result;

	return incResult;
}

/******************************************************************************
* JMP - Jump
******************************************************************************/
unsigned char CPU::JMP_AB(){
	if (debug) std::cout<< "JMP_AB: ";
	unsigned short addr = _AB();
	JMP(addr);
	return 3;
}
unsigned char CPU::JMP_IN(){
	if (debug) std::cout<< "JMP_IN: ";
	unsigned short addr = _AB();
	if ((addr & 0x00ff) == 0x00ff){
		addr = valueAt(addr) + (valueAt(addr-0xff) << 8);
	} else{
		addr = valueAt(addr) + (valueAt(addr+1) << 8);
	}
	JMP(addr);
	return 5;
}
void CPU::JMP(unsigned short operand){
	PC = operand;
}

/******************************************************************************
* JSR - Jump to Subroutine
******************************************************************************/
unsigned char CPU::JSR(){
	if (debug) std::cout<< "JSR: ";
	unsigned short addr = _AB();
	setValueAt(0x100 + SP--, (PC & 0xff00) >> 8);
	setValueAt(0x100 + SP--, (PC & 0x00ff));
	PC = addr;
	return 6;
}

/******************************************************************************
* KIL - Kill Program
******************************************************************************/
unsigned char CPU::KIL(){
	if (debug) std::cout<< "KIL: ";
	std::cout<< "Kill Instruction " << std::hex << int(valueAt(PC-1)) << " Executed" << "\n";
	return 0;
}

/******************************************************************************
* LAX - Load Accumulator then Transfer to X
******************************************************************************/
unsigned char CPU::LAX_ABY(){
	if (debug) std::cout<< "LAX_ABY: ";
	unsigned short addr = _ABY();
	LAX(valueAt(addr));
	return 4;
}
void CPU::LAX(unsigned char operand){
	if (operand == 0) P |= zeroMask; else P &= (0xff - zeroMask);
	if ((operand & 0x80) == 0x80) P |= negativeMask; else P &= (0xff - negativeMask);
	A = operand;
	X = A;
}

/******************************************************************************
* LDA - Load Accumulator
******************************************************************************/
unsigned char CPU::LDA_IM(){
	if (debug) std::cout<< "LDA_IM: ";
	LDA(valueAt(_IM()));
	return 2;
}
unsigned char CPU::LDA_ZP(){
	if (debug) std::cout<< "LDA_ZP: ";
	LDA(valueAt(_ZP()));
	return 3;
}
unsigned char CPU::LDA_ZPX(){
	if (debug) std::cout<< "LDA_ZPX: ";
	LDA(valueAt(_ZPX()));
	return 4;
}
unsigned char CPU::LDA_AB(){
	if (debug) std::cout<< "LDA_AB: ";
	LDA(valueAt(_AB()));
	return 4;
}
unsigned char CPU::LDA_ABX(){
	if (debug) std::cout<< "LDA_ABX: ";
	unsigned short addr = _ABX();
	LDA(valueAt(addr));
	if ((addr % 0x100) != (addr-X % 0x100)) return 5;
	else return 4;
}
unsigned char CPU::LDA_ABY(){
	if (debug) std::cout<< "LDA_ABY: ";
	unsigned short addr = _ABY();
	LDA(valueAt(addr));
	if ((addr % 0x100) != (addr-Y % 0x100)) return 5;
	else return 4;
}
unsigned char CPU::LDA_INX(){
	if (debug) std::cout<< "LDA_INX: ";
	LDA(valueAt(_INX()));
	return 6;
}
unsigned char CPU::LDA_INY(){
	if (debug) std::cout<< "LDA_INY: ";
	unsigned short addr = _INY();
	LDA(valueAt(addr));
	if ((addr % 0x100) != (addr-Y % 0x100)) return 6;
	else return 5;
}
void CPU::LDA(unsigned char operand){
	if (operand == 0) P |= zeroMask; else P &= (0xff - zeroMask);
	if ((operand & 0x80) == 0x80) P |= negativeMask; else P &= (0xff - negativeMask);
	A = operand;
}

/******************************************************************************
* LDX - Load X
******************************************************************************/
unsigned char CPU::LDX_IM(){
	if (debug) std::cout<< "LDX_IM: ";
	LDX(valueAt(_IM()));
	return 2;
}
unsigned char CPU::LDX_ZP(){
	if (debug) std::cout<< "LDX_ZP: ";
	LDX(valueAt(_ZP()));
	return 3;
}
unsigned char CPU::LDX_ZPY(){
	if (debug) std::cout<< "LDX_ZPY: ";
	LDX(valueAt(_ZPY()));
	return 4;
}
unsigned char CPU::LDX_AB(){
	if (debug) std::cout<< "LDX_AB: ";
	LDX(valueAt(_AB()));
	return 4;
}
unsigned char CPU::LDX_ABY(){
	if (debug) std::cout<< "LDX_ABY: ";
	unsigned short addr = _ABY();
	LDX(valueAt(addr));
	if ((addr % 0x100) != (addr-Y % 0x100)) return 5;
	else return 4;
}
void CPU::LDX(unsigned char operand){
	if (operand == 0) P |= zeroMask; else P &= (0xff - zeroMask);
	if ((operand & 0x80) == 0x80) P |= negativeMask; else P &= (0xff - negativeMask);
	X = operand;
}

/******************************************************************************
* LDY - Load Y
******************************************************************************/
unsigned char CPU::LDY_IM(){
	if (debug) std::cout<< "LDY_IM: ";
	LDY(valueAt(_IM()));
	return 2;
}
unsigned char CPU::LDY_ZP(){
	if (debug) std::cout<< "LDY_ZP: ";
	LDY(valueAt(_ZP()));
	return 3;
}
unsigned char CPU::LDY_ZPX(){
	if (debug) std::cout<< "LDY_ZPX: ";
	LDY(valueAt(_ZPX()));
	return 4;
}
unsigned char CPU::LDY_AB(){
	if (debug) std::cout<< "LDY_AB: ";
	LDY(valueAt(_AB()));
	return 4;
}
unsigned char CPU::LDY_ABX(){
	if (debug) std::cout<< "LDY_ABX: ";
	unsigned short addr = _ABX();
	LDY(valueAt(addr));
	if ((addr % 0x100) != (addr-X % 0x100)) return 5;
	else return 4;
}
void CPU::LDY(unsigned char operand){
	if (operand == 0) P |= zeroMask; else P &= (0xff - zeroMask);
	if ((operand & 0x80) == 0x80) P |= negativeMask; else P &= (0xff - negativeMask);
	Y = operand;
}

/******************************************************************************
* LSR - Logical Shift Right
******************************************************************************/
unsigned char CPU::LSR_ACC(){
	A = LSR(A);
	return 2;
}
unsigned char CPU::LSR_ZP(){
	unsigned short addr = _ZP();
	setValueAt(addr, LSR(valueAt(addr)));
	return 5;
}
unsigned char CPU::LSR_ZPX(){
	unsigned short addr = _ZPX();
	setValueAt(addr, LSR(valueAt(addr)));
	return 6;
}
unsigned char CPU::LSR_AB(){
	unsigned short addr = _AB();
	setValueAt(addr, LSR(valueAt(addr)));
	return 6;
}
unsigned char CPU::LSR_ABX(){
	unsigned short addr = _ABX();
	setValueAt(addr, LSR(valueAt(addr)));
	return 7;
}
unsigned char CPU::LSR(unsigned char operand){
	unsigned char result = operand >> 1;

	if ((operand & 0x01) == 0x01) P |= carryMask; else P &= (0xff - carryMask);
	if (result == 0) P |= zeroMask; else P &= (0xff - zeroMask);
	if ((result & 0x80) == 0x80) P |= negativeMask; else P &= (0xff - negativeMask);
	return result;
}

/******************************************************************************
* NOP - No Operation
******************************************************************************/
unsigned char CPU::NOP(){
	if (debug) std::cout<< "NOP: ";
	return 2;
}

/******************************************************************************
* ORA - Logical OR
******************************************************************************/
unsigned char CPU::ORA_IM(){
	if (debug) std::cout<< "ORA_IM: ";
	ORA(valueAt(_IM()));
	return 2;
}
unsigned char CPU::ORA_ZP(){
	if (debug) std::cout<< "ORA_ZP: ";
	ORA(valueAt(_ZP()));
	return 3;
}
unsigned char CPU::ORA_ZPX(){
	if (debug) std::cout<< "ORA_ZPX: ";
	ORA(valueAt(_ZPX()));
	return 4;
}
unsigned char CPU::ORA_AB(){
	if (debug) std::cout<< "ORA_AB: ";
	ORA(valueAt(_AB()));
	return 4;
}
unsigned char CPU::ORA_ABX(){
	if (debug) std::cout<< "ORA_ABX: ";
	unsigned short addr = _ABX();
	ORA(valueAt(addr));
	if ((addr % 0x100) != (addr-X % 0x100)) return 5;
	else return 4;
}
unsigned char CPU::ORA_ABY(){
	if (debug) std::cout<< "ORA_ABY: ";
	unsigned short addr = _ABY();
	ORA(valueAt(addr));
	if ((addr % 0x100) != (addr-Y % 0x100)) return 5;
	else return 4;
}
unsigned char CPU::ORA_INX(){
	if (debug) std::cout<< "ORA_INX: ";
	ORA(valueAt(_INX()));
	return 6;
}
unsigned char CPU::ORA_INY(){
	if (debug) std::cout<< "ORA_INY: ";
	unsigned short addr = _INY();
	ORA(valueAt(addr));
	if ((addr % 0x100) != (addr-Y % 0x100)) return 6;
	else return 5;
}
void CPU::ORA(unsigned char operand){
	unsigned char op1 = A;
	unsigned char op2 = operand;
	unsigned char result = op1 | op2;

	if (result == 0) P |= zeroMask; else P &= (0xff - zeroMask);
	if ((result & 0x80) == 0x80) P |= negativeMask; else P &= (0xff - negativeMask);
	A = result;
}

/******************************************************************************
* PHA - Push Accumulator
******************************************************************************/
unsigned char CPU::PHA(){
	if (debug) std::cout<< "PHA: ";
	setValueAt(0x100 + SP--, A);
	return 3;
}

/******************************************************************************
* PHP - Push Processor Status
******************************************************************************/
unsigned char CPU::PHP(){
	if (debug) std::cout<< "PHP: ";
	setValueAt(0x100 + SP--, P);
	return 3;
}

/******************************************************************************
* PLA - Pull Accumulator
******************************************************************************/
unsigned char CPU::PLA(){
	if (debug) std::cout<< "PLA: ";
	A = valueAt(0x100 + ++SP);
	return 4;
}

/******************************************************************************
* PLA - Pull Processor Status
******************************************************************************/
unsigned char CPU::PLP(){
	if (debug) std::cout<< "PLP: ";
	P = valueAt(0x100 + ++SP);
	return 4;
}

/******************************************************************************
* RLA - Rotate Left then AND
******************************************************************************/
unsigned char CPU::RLA_ABX(){
	if (debug) std::cout<< "RLA_ABX: ";
	unsigned short addr = _ABX();
	setValueAt(addr, RLA(valueAt(addr)));
	return 7;
}
unsigned char CPU::RLA(unsigned char operand){
	unsigned char rolResult = operand << 1;
	if ((P & carryMask) == carryMask) rolResult += 1;

	if ((operand & 0x80) == 0x80) P |= carryMask; else P &= (0xff - carryMask);
	if (rolResult == 0) P |= zeroMask; else P &= (0xff - zeroMask);
	if ((rolResult & 0x80) == 0x80) P |= negativeMask; else P &= (0xff - negativeMask);

	unsigned char op1 = A;
	unsigned char op2 = rolResult;
	unsigned char result = op1 & op2;

	if (result == 0) P |= zeroMask; else P &= (0xff - zeroMask);
	if ((result & 0x80) == 0x80) P |= negativeMask; else P &= (0xff - negativeMask);
	A = result;
	return rolResult;
}

/******************************************************************************
* ROL - Rotate Left
******************************************************************************/
unsigned char CPU::ROL_ACC(){
	if (debug) std::cout<< "ROL_ACC: ";
	A = ROL(A);
	return 2;
}
unsigned char CPU::ROL_ZP(){
	if (debug) std::cout<< "ROL_ZP: ";
	unsigned short addr = _ZP();
	setValueAt(addr, ROL(valueAt(addr)));
	return 5;
}
unsigned char CPU::ROL_ZPX(){
	if (debug) std::cout<< "ROL_ZPX: ";
	unsigned short addr = _ZPX();
	setValueAt(addr, ROL(valueAt(addr)));
	return 6;
}
unsigned char CPU::ROL_AB(){
	if (debug) std::cout<< "ROL_AB: ";
	unsigned short addr = _AB();
	setValueAt(addr, ROL(valueAt(addr)));
	return 6;
}
unsigned char CPU::ROL_ABX(){
	if (debug) std::cout<< "ROL_ABX: ";
	unsigned short addr = _ABX();
	setValueAt(addr, ROL(valueAt(addr)));
	return 7;
}
unsigned char CPU::ROL(unsigned char operand){
	unsigned char result = operand << 1;
	if ((P & carryMask) == carryMask) result += 1;

	if ((operand & 0x80) == 0x80) P |= carryMask; else P &= (0xff - carryMask);
	if (result == 0) P |= zeroMask; else P &= (0xff - zeroMask);
	if ((result & 0x80) == 0x80) P |= negativeMask; else P &= (0xff - negativeMask);
	return result;
}

/******************************************************************************
* ROR - Rotate Right
******************************************************************************/
unsigned char CPU::ROR_ACC(){
	if (debug) std::cout<< "ROR_ACC: ";
	A = ROR(A);
	return 2;
}
unsigned char CPU::ROR_ZP(){
	if (debug) std::cout<< "ROR_ZP: ";
	unsigned short addr = _ZP();
	setValueAt(addr, ROR(valueAt(addr)));
	return 5;
}
unsigned char CPU::ROR_ZPX(){
	if (debug) std::cout<< "ROR_ZPX: ";
	unsigned short addr = _ZPX();
	setValueAt(addr, ROR(valueAt(addr)));
	return 6;
}
unsigned char CPU::ROR_AB(){
	if (debug) std::cout<< "ROR_AB: ";
	unsigned short addr = _AB();
	setValueAt(addr, ROR(valueAt(addr)));
	return 6;
}
unsigned char CPU::ROR_ABX(){
	if (debug) std::cout<< "ROR_ABX: ";
	unsigned short addr = _ABX();
	setValueAt(addr, ROR(valueAt(addr)));
	return 7;
}
unsigned char CPU::ROR(unsigned char operand){
	unsigned char result = operand >> 1;
	if ((P & carryMask) == carryMask) result += 0x80;

	if ((operand & 0x01) == 0x01) P |= carryMask; else P &= (0xff - carryMask);
	if (result == 0) P |= zeroMask; else P &= (0xff - zeroMask);
	if ((result & 0x80) == 0x80) P |= negativeMask; else P &= (0xff - negativeMask);
	return result;
}

/******************************************************************************
* RTI - Return from Interrupt
******************************************************************************/
unsigned char CPU::RTI(){
	if (debug) std::cout<< "RTI: ";
	P = valueAt(0x100 + ++SP);
	PC = valueAt(0x100 + ++SP);
	PC += valueAt(0x100 + ++SP) << 8;
	return 6;
}

/******************************************************************************
* RTS - Return from Subroutine
******************************************************************************/
unsigned char CPU::RTS(){
	if (debug) std::cout<< "RTS: ";
	PC = valueAt(0x100 + ++SP);
	PC += valueAt(0x100 + ++SP) << 8;
	return 6;
}

/******************************************************************************
* SAX - Store A AND X
******************************************************************************/
unsigned char CPU::SAX_ZP(){
	if (debug) std::cout<< "SAX_ZP: ";
	unsigned short addr = _ZP();
	setValueAt(addr, SAX());
	return 3;
}
unsigned char CPU::SAX_INX(){
	if (debug) std::cout<< "SAX_INX: ";
	unsigned short addr = _INX();
	setValueAt(addr, SAX());
	return 6;
}
unsigned char CPU::SAX(){
	return A & X;
}

/******************************************************************************
* SBC - Subtract With Carry
******************************************************************************/
unsigned char CPU::SBC_IM(){
	if (debug) std::cout<< "SBC_IM: ";
	SBC(valueAt(_IM()));
	return 2;
}
unsigned char CPU::SBC_ZP(){
	if (debug) std::cout<< "SBC_ZP: ";
	SBC(valueAt(_ZP()));
	return 3;
}
unsigned char CPU::SBC_ZPX(){
	if (debug) std::cout<< "SBC_ZPX: ";
	SBC(valueAt(_ZPX()));
	return 4;
}
unsigned char CPU::SBC_AB(){
	if (debug) std::cout<< "SBC_AB: ";
	SBC(valueAt(_AB()));
	return 4;
}
unsigned char CPU::SBC_ABX(){
	if (debug) std::cout<< "SBC_ABX: ";
	unsigned short addr = _ABX();
	SBC(valueAt(addr));
	if ((addr % 0x100) != (addr-X % 0x100)) return 5;
	else return 4;
}
unsigned char CPU::SBC_ABY(){
	if (debug) std::cout<< "SBC_ABY: ";
	unsigned short addr = _ABY();
	SBC(valueAt(addr));
	if ((addr % 0x100) != (addr-Y % 0x100)) return 5;
	else return 4;
}
unsigned char CPU::SBC_INX(){
	if (debug) std::cout<< "SBC_INX: ";
	SBC(valueAt(_INX()));
	return 6;
}
unsigned char CPU::SBC_INY(){
	if (debug) std::cout<< "SBC_INY: ";
	unsigned short addr = _INY();
	SBC(valueAt(addr));
	if ((addr % 0x100) != (addr-Y % 0x100)) return 6;
	else return 5;
}
void CPU::SBC(unsigned char operand){
	char op1 = char(A);
	char op2 = char(operand);
	char result = op1 - op2 + (P & carryMask) - 1;

	if (result == 0) P |= zeroMask; else P &= (0xff - zeroMask);
	if ((result & 0x80) == 0x80) P |= negativeMask; else P &= (0xff - negativeMask);
	if (op1 > 0 && op2 > 0 && (result & 0x80) == 0x80) P |= overflowMask;
	else if (op1 < 0 && op2 < 0 && (result & 0x80) == 0x00) P |= overflowMask;
	else P &= (0xff - overflowMask);
	if (((unsigned short)(op1) + (unsigned short)(op2) & 0x100) == 0x100) P &= (0xff - carryMask);
	else P |= carryMask;
	A = (unsigned char)result;
}

/******************************************************************************
* SEC - Set Carry Flag
******************************************************************************/
unsigned char CPU::SEC(){
	if (debug) std::cout<< "SEC: ";
	P |= carryMask;
	return 2;
}

/******************************************************************************
* SED - Set Decimal Flag
******************************************************************************/
unsigned char CPU::SED(){
	if (debug) std::cout<< "SED: ";
	P |= decimalMask;
	return 2;
}

/******************************************************************************
* SEI - Set Interrupt Disable Flag
******************************************************************************/
unsigned char CPU::SEI(){
	if (debug) std::cout<< "SEI: ";
	P |= interruptDisableMask;
	return 2;
}

/******************************************************************************
* SKB - Skip Byte
******************************************************************************/
unsigned char CPU::SKB(){
	if (debug) std::cout<< "SKB: ";
	PC++;
	return 2;
}

/******************************************************************************
* SLO - Shift Left then OR
******************************************************************************/
unsigned char CPU::SLO_ZPX(){
	if (debug) std::cout<< "SLO_ZPX: ";
	unsigned short addr = _ZPX();
	setValueAt(addr, SLO(valueAt(addr)));
	return 6;
}
unsigned char CPU::SLO_AB(){
	if (debug) std::cout<< "SLO_AB: ";
	unsigned short addr = _AB();
	setValueAt(addr, SLO(valueAt(addr)));
	return 6;
}
unsigned char CPU::SLO_ABX(){
	if (debug) std::cout<< "SLO_ABX: ";
	unsigned short addr = _ABX();
	setValueAt(addr, SLO(valueAt(addr)));
	return 7;
}
unsigned char CPU::SLO_INX(){
	if (debug) std::cout<< "SLO_INX: ";
	unsigned short addr = _INX();
	setValueAt(addr, SLO(valueAt(addr)));
	return 8;
}
unsigned char CPU::SLO(unsigned char operand){
	unsigned char shiftResult = operand << 1;

	if ((operand & 0x80) == 0x80) P |= carryMask; else P &= (0xff - carryMask);
	if (shiftResult == 0) P |= zeroMask; else P &= (0xff - zeroMask);
	if ((shiftResult & 0x80) == 0x80) P |= negativeMask; else P &= (0xff - negativeMask);

	unsigned char op1 = A;
	unsigned char op2 = shiftResult;
	unsigned char result = op1 | op2;

	if (result == 0) P |= zeroMask; else P &= (0xff - zeroMask);
	if ((result & 0x80) == 0x80) P |= negativeMask; else P &= (0xff - negativeMask);
	A = result;
	return shiftResult;
}

/******************************************************************************
* SRE - Shift Right then EOR
******************************************************************************/
unsigned char CPU::SRE_AB(){
	if (debug) std::cout<< "SRE_AB: ";
	unsigned short addr = _AB();
	setValueAt(addr, SRE(valueAt(addr)));
	return 6;
}
unsigned char CPU::SRE_INY(){
	if (debug) std::cout<< "SRE_INY: ";
	unsigned short addr = _INY();
	setValueAt(addr, SRE(valueAt(addr)));
	return 8;
}
unsigned char CPU::SRE(unsigned char operand){
	unsigned char shiftResult = operand >> 1;

	if ((operand & 0x01) == 0x01) P |= carryMask; else P &= (0xff - carryMask);
	if (shiftResult == 0) P |= zeroMask; else P &= (0xff - zeroMask);
	if ((shiftResult & 0x80) == 0x80) P |= negativeMask; else P &= (0xff - negativeMask);

	unsigned char op1 = A;
	unsigned char op2 = shiftResult;
	unsigned char result = op1 ^ op2;

	if (result == 0) P |= zeroMask; else P &= (0xff - zeroMask);
	if ((result & 0x80) == 0x80) P |= negativeMask; else P &= (0xff - negativeMask);
	A = result;
	return shiftResult;
}

/******************************************************************************
* STA - Store Accumulator
******************************************************************************/
unsigned char CPU::STA_ZP(){
	if (debug) std::cout<< "STA_ZP: ";
	STA(_ZP());
	return 3;
}
unsigned char CPU::STA_ZPX(){
	if (debug) std::cout<< "STA_ZPX: ";
	STA(_ZPX());
	return 4;
}
unsigned char CPU::STA_AB(){
	if (debug) std::cout<< "STA_AB: ";
	STA(_AB());
	return 4;
}
unsigned char CPU::STA_ABX(){
	if (debug) std::cout<< "STA_ABX: ";
	STA(_ABX());
	return 5;
}
unsigned char CPU::STA_ABY(){
	if (debug) std::cout<< "STA_ABY: ";
	STA(_ABY());
	return 5;
}
unsigned char CPU::STA_INX(){
	if (debug) std::cout<< "STA_INX: ";
	STA(_INX());
	return 6;
}
unsigned char CPU::STA_INY(){
	if (debug) std::cout<< "STA_INY: ";
	STA(_INY());
	return 6;
}
void CPU::STA(unsigned short operand){
	setValueAt(operand, A);
}

/******************************************************************************
* STX - Store X
******************************************************************************/
unsigned char CPU::STX_ZP(){
	if (debug) std::cout<< "STX_ZP: ";
	STX(_ZP());
	return 3;
}
unsigned char CPU::STX_ZPY(){
	if (debug) std::cout<< "STX_ZPY: ";
	STX(_ZPY());
	return 4;
}
unsigned char CPU::STX_AB(){
	if (debug) std::cout<< "STX_AB: ";
	STX(_AB());
	return 4;
}
void CPU::STX(unsigned short operand){
	setValueAt(operand, X);
}

/******************************************************************************
* STY - Store Y
******************************************************************************/
unsigned char CPU::STY_ZP(){
	if (debug) std::cout<< "STY_ZP: ";
	STY(_ZP());
	return 3;
}
unsigned char CPU::STY_ZPX(){
	if (debug) std::cout<< "STY_ZPX: ";
	STY(_ZPX());
	return 4;
}
unsigned char CPU::STY_AB(){
	if (debug) std::cout<< "STY_AB: ";
	STY(_AB());
	return 4;
}
void CPU::STY(unsigned short operand){
	setValueAt(operand, Y);
}

/******************************************************************************
* TAX - Transfer Accumulator to X
******************************************************************************/
unsigned char CPU::TAX(){
	if (debug) std::cout<< "TAX: ";
	if (A == 0) P |= zeroMask; else P &= (0xff - zeroMask);
	if ((A & 0x80) == 0x80) P |= negativeMask; else P &= (0xff - negativeMask);
	X = A;
	return 2;
}

/******************************************************************************
* TAY - Transfer Accumulator to Y
******************************************************************************/
unsigned char CPU::TAY(){
	if (debug) std::cout<< "TAY: ";
	if (A == 0) P |= zeroMask; else P &= (0xff - zeroMask);
	if ((A & 0x80) == 0x80) P |= negativeMask; else P &= (0xff - negativeMask);
	Y = A;
	return 2;
}

/******************************************************************************
* TSX - Transfer Stack Pointer to X
******************************************************************************/
unsigned char CPU::TSX(){
	if (debug) std::cout<< "TSX: ";
	if (SP == 0) P |= zeroMask; else P &= (0xff - zeroMask);
	if ((SP & 0x80) == 0x80) P |= negativeMask; else P &= (0xff - negativeMask);
	X = SP;
	return 2;
}

/******************************************************************************
* TXA - Transfer X to Accumulator
******************************************************************************/
unsigned char CPU::TXA(){
	if (debug) std::cout<< "TXA: ";
	if (X == 0) P |= zeroMask; else P &= (0xff - zeroMask);
	if ((X & 0x80) == 0x80) P |= negativeMask; else P &= (0xff - negativeMask);
	A = X;
	return 2;
}

/******************************************************************************
* TXS - Transfer X to Stack Pointer
******************************************************************************/
unsigned char CPU::TXS(){
	if (debug) std::cout<< "TXS: ";
	if (X == 0) P |= zeroMask; else P &= (0xff - zeroMask);
	if ((X & 0x80) == 0x80) P |= negativeMask; else P &= (0xff - negativeMask);
	SP = X;
	return 2;
}

/******************************************************************************
* TYA - Transfer Y to Accumulator
******************************************************************************/
unsigned char CPU::TYA(){
	if (debug) std::cout<< "TYA: ";
	if (Y == 0) P |= zeroMask; else P &= (0xff - zeroMask);
	if ((Y & 0x80) == 0x80) P |= negativeMask; else P &= (0xff - negativeMask);
	A = Y;
	return 2;
}