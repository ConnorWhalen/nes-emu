#include "execute.h"

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
unsigned char Execute::execute(unsigned char opcode){
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