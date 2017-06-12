#ifndef CPU_H
#define CPU_H

#include <vector>
#include <iostream>
#include <fstream>
#include <string>

#include "ppu.h"
#include "apu.h"
#include "controller.h"

class PPU;
class APU;

class CPU {
	friend class PPU;

	public:
		static constexpr unsigned char carryMask = 0x01;
		static constexpr unsigned char zeroMask = 0x02;
		static constexpr unsigned char interruptDisableMask = 0x04;
		static constexpr unsigned char decimalMask = 0x08;
		static constexpr unsigned char breakMask = 0x10;
		static constexpr unsigned char overflowMask = 0x40;
		static constexpr unsigned char negativeMask = 0x80;

		CPU(std::vector<unsigned char>* romBytes, unsigned char* cartRAM, unsigned short programBankCount,
			unsigned short mapper, PPU* ppu, APU* apu, Controller* player1, Controller* player2);
		void reset();
		void nmi();
		void irq();
		void setDebug();

		unsigned char execute();

		void writeRamToFile(std::string filename);

	private:
		unsigned short PC;
		unsigned char SP;
		unsigned char X;
		unsigned char Y;
		unsigned char A;
		unsigned char P;

		std::vector<unsigned char>* romBytes;
		unsigned char* RAM;
		unsigned char* cartRAM;
		unsigned char programBank0;
		unsigned char programBank1;
		unsigned short programBankCount;
		unsigned short mapper;

		PPU* ppu;
		APU* apu;
		Controller* player1;
		Controller* player2;

		bool debug;

		unsigned char valueAt(unsigned short address);
		void setValueAt(unsigned short address, unsigned char value);

		unsigned short _IM();
		unsigned short _ZP();
		unsigned short _ZPX();
		unsigned short _ZPY();
		unsigned short _AB();
		unsigned short _ABX();
		unsigned short _ABY();
		unsigned short _INX();
		unsigned short _INY();

		unsigned char ADC_IM();
		unsigned char ADC_ZP();
		unsigned char ADC_ZPX();
		unsigned char ADC_AB();
		unsigned char ADC_ABX();
		unsigned char ADC_ABY();
		unsigned char ADC_INX();
		unsigned char ADC_INY();
		void ADC(unsigned char operand);

		unsigned char ANC();

		unsigned char AND_IM();
		unsigned char AND_ZP();
		unsigned char AND_ZPX();
		unsigned char AND_AB();
		unsigned char AND_ABX();
		unsigned char AND_ABY();
		unsigned char AND_INX();
		unsigned char AND_INY();
		void AND(unsigned char operand);

		unsigned char ASL_ACC();
		unsigned char ASL_ZP();
		unsigned char ASL_ZPX();
		unsigned char ASL_AB();
		unsigned char ASL_ABX();
		unsigned char ASL(unsigned char operand);

		unsigned char BCC();

		unsigned char BCS();

		unsigned char BEQ();

		unsigned char BIT_ZP();
		unsigned char BIT_AB();
		void BIT(unsigned char operand);

		unsigned char BMI();

		unsigned char BNE();

		unsigned char BPL();

		unsigned char BRK();

		unsigned char BVC();

		unsigned char BVS();

		unsigned char CLC();

		unsigned char CLD();

		unsigned char CLI();

		unsigned char CLV();

		unsigned char CMP_IM();
		unsigned char CMP_ZP();
		unsigned char CMP_ZPX();
		unsigned char CMP_AB();
		unsigned char CMP_ABX();
		unsigned char CMP_ABY();
		unsigned char CMP_INX();
		unsigned char CMP_INY();
		void CMP(unsigned char operand);

		unsigned char CPX_IM();
		unsigned char CPX_ZP();
		unsigned char CPX_AB();
		void CPX(unsigned char operand);

		unsigned char CPY_IM();
		unsigned char CPY_ZP();
		unsigned char CPY_AB();
		void CPY(unsigned char operand);

		unsigned char DCP_ZP();
		unsigned char DCP_ABX();
		unsigned char DCP_ABY();
		unsigned char DCP(unsigned char operand);

		unsigned char DEC_ZP();
		unsigned char DEC_ZPX();
		unsigned char DEC_AB();
		unsigned char DEC_ABX();
		unsigned char DEC(unsigned char operand);

		unsigned char DEX();

		unsigned char DEY();

		unsigned char EOR_IM();
		unsigned char EOR_ZP();
		unsigned char EOR_ZPX();
		unsigned char EOR_AB();
		unsigned char EOR_ABX();
		unsigned char EOR_ABY();
		unsigned char EOR_INX();
		unsigned char EOR_INY();
		void EOR(unsigned char operand);

		unsigned char IGN_ZP();
		unsigned char IGN_ZPX();
		unsigned char IGN_ABX();
		void IGN(unsigned short operand);

		unsigned char INC_ZP();
		unsigned char INC_ZPX();
		unsigned char INC_AB();
		unsigned char INC_ABX();
		unsigned char INC(unsigned char operand);

		unsigned char INX();

		unsigned char INY();

		unsigned char ISC_ZPX();
		unsigned char ISC_AB();
		unsigned char ISC_ABX();
		unsigned char ISC_ABY();
		unsigned char ISC_INX();
		unsigned char ISC(unsigned char operand);

		unsigned char JMP_AB();
		unsigned char JMP_IN();
		void JMP(unsigned short operand);

		unsigned char JSR();

		unsigned char KIL();

		unsigned char LAX_ABY();
		void LAX(unsigned char operand);

		unsigned char LDA_IM();
		unsigned char LDA_ZP();
		unsigned char LDA_ZPX();
		unsigned char LDA_AB();
		unsigned char LDA_ABX();
		unsigned char LDA_ABY();
		unsigned char LDA_INX();
		unsigned char LDA_INY();
		void LDA(unsigned char operand);

		unsigned char LDX_IM();
		unsigned char LDX_ZP();
		unsigned char LDX_ZPY();
		unsigned char LDX_AB();
		unsigned char LDX_ABY();
		void LDX(unsigned char operand);

		unsigned char LDY_IM();
		unsigned char LDY_ZP();
		unsigned char LDY_ZPX();
		unsigned char LDY_AB();
		unsigned char LDY_ABX();
		void LDY(unsigned char operand);

		unsigned char LSR_ACC();
		unsigned char LSR_ZP();
		unsigned char LSR_ZPX();
		unsigned char LSR_AB();
		unsigned char LSR_ABX();
		unsigned char LSR(unsigned char operand);

		unsigned char NOP();

		unsigned char ORA_IM();
		unsigned char ORA_ZP();
		unsigned char ORA_ZPX();
		unsigned char ORA_AB();
		unsigned char ORA_ABX();
		unsigned char ORA_ABY();
		unsigned char ORA_INX();
		unsigned char ORA_INY();
		void ORA(unsigned char operand);

		unsigned char PHA();

		unsigned char PHP();

		unsigned char PLA();

		unsigned char PLP();

		unsigned char RLA_ABX();
		unsigned char RLA(unsigned char operand);

		unsigned char ROL_ACC();
		unsigned char ROL_ZP();
		unsigned char ROL_ZPX();
		unsigned char ROL_AB();
		unsigned char ROL_ABX();
		unsigned char ROL(unsigned char operand);

		unsigned char ROR_ACC();
		unsigned char ROR_ZP();
		unsigned char ROR_ZPX();
		unsigned char ROR_AB();
		unsigned char ROR_ABX();
		unsigned char ROR(unsigned char operand);

		unsigned char RTI();

		unsigned char RTS();

		unsigned char SAX_ZP();
		unsigned char SAX_INX();
		unsigned char SAX();

		unsigned char SBC_IM();
		unsigned char SBC_ZP();
		unsigned char SBC_ZPX();
		unsigned char SBC_AB();
		unsigned char SBC_ABX();
		unsigned char SBC_ABY();
		unsigned char SBC_INX();
		unsigned char SBC_INY();
		void SBC(unsigned char operand);

		unsigned char SEC();

		unsigned char SED();

		unsigned char SEI();

		unsigned char SKB();

		unsigned char SLO_ZPX();
		unsigned char SLO_AB();
		unsigned char SLO_ABX();
		unsigned char SLO_INX();
		unsigned char SLO(unsigned char operand);

		unsigned char SRE_AB();
		unsigned char SRE_INY();
		unsigned char SRE(unsigned char operand);

		unsigned char STA_ZP();
		unsigned char STA_ZPX();
		unsigned char STA_AB();
		unsigned char STA_ABX();
		unsigned char STA_ABY();
		unsigned char STA_INX();
		unsigned char STA_INY();
		void STA(unsigned short operand);

		unsigned char STX_ZP();
		unsigned char STX_ZPY();
		unsigned char STX_AB();
		void STX(unsigned short operand);

		unsigned char STY_ZP();
		unsigned char STY_ZPX();
		unsigned char STY_AB();
		void STY(unsigned short operand);

		unsigned char TAX();

		unsigned char TAY();

		unsigned char TSX();

		unsigned char TXA();

		unsigned char TXS();

		unsigned char TYA();
};

#endif