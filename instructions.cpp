#include "instructions.h"

/******************************************************************************
* ADC - Add with Carry
******************************************************************************/
unsigned char ADC_IM(){
	if (debug) std::cout<< "ADC_IM: ";
	if (debug) std::cout<< "Add " << int(valueAt(PC)) << " and " << int(A) << "\n";
	ADC(valueAt(PC++));
	return 2;
}
unsigned char ADC_ZP(){
	if (debug) std::cout<< "ADC_ZP: ";
	if (debug) std::cout<< "Add " << int(valueAt(valueAt(PC))) << " and " << int(A) << "\n";
	ADC(valueAt(valueAt(PC++)));
	return 3;
}
unsigned char ADC_ZPX(){
	if (debug) std::cout<< "ADC_ZPX: ";
	if (debug) std::cout<< "Add " << int(valueAt((valueAt(PC) + X) % 0x100)) << " (addr " << int((valueAt(PC) + X) % 0x100) << ") and " << int(A) << "\n";
	ADC(valueAt((valueAt(PC++) + X) % 0x100));
	return 4;
}
unsigned char ADC_AB(){
	if (debug) std::cout<< "ADC_AB: ";
	if (debug) std::cout<< "Add " << int(valueAt(valueAt(PC) + ((valueAt(PC+1)) << 8))) << " and " << int(A) << "\n";
	ADC(valueAt(valueAt(PC) + ((valueAt(PC+1)) << 8)));
	PC += 2;
	return 4;
}
unsigned char ADC_ABX(){
	if (debug) std::cout<< "ADC_ABX: ";
	unsigned short addr = valueAt(PC) + (valueAt(PC+1) << 8);
	if (debug) std::cout<< "Add " << int(valueAt(addr + X)) << " (addr " << int(addr) << ") and " << int(A) << "\n";
	ADC(valueAt(addr + X));
	PC += 2;
	if ((addr % 0x100) + X > 0xff) return 5;
	else return 4;
}
unsigned char ADC_ABY(){
	if (debug) std::cout<< "ADC_ABY: ";
	unsigned short addr = valueAt(PC) + (valueAt(PC+1) << 8);
	if (debug) std::cout<< "Add " << int(valueAt(addr + Y)) << " (addr " << int(addr) << ") and " << int(A) << "\n";
	ADC(valueAt(addr + Y));
	PC += 2;
	if ((addr % 0x100) + Y > 0xff) return 5;
	else return 4;
}
unsigned char ADC_INX(){
	if (debug) std::cout<< "ADC_INX: ";
	unsigned short addr = ((valueAt(PC++) + X) % 0x100);
	addr = valueAt(addr) + (valueAt((addr+1) % 0x100) << 8);
	if (debug) std::cout<< "Add " << int(valueAt(addr)) << " (addr " << int(addr) << ") and " << int(A) << "\n";
	ADC(valueAt(addr));
	return 6;
}
unsigned char ADC_INY(){
	if (debug) std::cout<< "ADC_INY: ";
	unsigned short addr = valueAt(valueAt(PC)) + (valueAt(valueAt(PC)+1) << 8);
	if (debug) std::cout<< "Add " << int(valueAt(addr + Y)) << " (addr " << int(addr+Y) << ") and " << int(A) << "\n";
	ADC(valueAt(addr + Y));
	PC++;
	if ((addr % 0x100) + Y > 0xff) return 6;
	else return 5;
}
void ADC(unsigned char operand){
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
unsigned char ANC(){
	if (debug) std::cout<< "ANC: Add " << int(valueAt(PC)) << " and " << int(A) << "\n";
	unsigned char operand = valueAt(PC++);

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
unsigned char AND_IM(){
	if (debug) std::cout<< "AND_IM: ";
	if (debug) std::cout<< "And " << int(valueAt(PC)) << " and " << int(A) << "\n";
	AND(valueAt(PC++));
	return 2;
}
unsigned char AND_ZP(){
	if (debug) std::cout<< "AND_ZP: ";
	if (debug) std::cout<< "And " << int(valueAt(valueAt(PC))) << " and " << int(A) << "\n";
	AND(valueAt(valueAt(PC++)));
	return 3;
}
unsigned char AND_ZPX(){
	if (debug) std::cout<< "AND_ZPX: ";
	if (debug) std::cout<< "And " << int(valueAt((valueAt(PC) + X) % 0x100)) << " (addr " << int((valueAt(PC) + X) % 0x100) << ") and " << int(A) << "\n";
	AND(valueAt((valueAt(PC++) + X) % 0x100));
	return 4;
}
unsigned char AND_AB(){
	if (debug) std::cout<< "AND_AB: ";
	if (debug) std::cout<< "And " << int(valueAt(valueAt(PC) + ((valueAt(PC+1)) << 8))) << " and " << int(A) << "\n";
	AND(valueAt(valueAt(PC) + ((valueAt(PC+1)) << 8)));
	PC += 2;
	return 4;
}
unsigned char AND_ABX(){
	if (debug) std::cout<< "AND_ABX: ";
	unsigned short addr = valueAt(PC) + (valueAt(PC+1) << 8);
	if (debug) std::cout<< "And " << int(valueAt(addr + X)) << " (addr " << int(addr) << ") and " << int(A) << "\n";
	AND(valueAt(addr + X));
	PC += 2;
	if ((addr % 0x100) + X > 0xff) return 5;
	else return 4;
}
unsigned char AND_ABY(){
	if (debug) std::cout<< "AND_ABY: ";
	unsigned short addr = valueAt(PC) + (valueAt(PC+1) << 8);
	std::cout<< "And " << int(valueAt(addr + Y)) << " (addr " << int(addr) << ") and " << int(A) << "\n";
	AND(valueAt(addr + Y));
	PC += 2;
	if ((addr % 0x100) + Y > 0xff) return 5;
	else return 4;
}
unsigned char AND_INX(){
	if (debug) std::cout<< "AND_INX: ";
	unsigned short addr = ((valueAt(PC++) + X) % 0x100);
	addr = valueAt(addr) + (valueAt((addr+1) % 0x100) << 8);
	if (debug) std::cout<< "Add " << int(valueAt(addr)) << " (addr " << int(addr) << ") and " << int(A) << "\n";
	AND(valueAt(addr));
	return 6;
}
unsigned char AND_INY(){
	if (debug) std::cout<< "AND_INY: ";
	unsigned short addr = valueAt(valueAt(PC)) + (valueAt(valueAt(PC)+1) << 8);
	if (debug) std::cout<< "And " << int(valueAt(addr + Y)) << " (addr " << int(addr+Y) << ") and " << int(A) << "\n";
	AND(valueAt(addr + Y));
	PC++;
	if ((addr % 0x100) + Y > 0xff) return 6;
	else return 5;
}
void AND(unsigned char operand){
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
unsigned char ASL_ACC(){
	if (debug) std::cout<< "ASL_ACC: \n";
	A = ASL(A);
	return 2;
}
unsigned char ASL_ZP(){
	if (debug) std::cout<< "ASL_ZP: ";
	unsigned short addr = valueAt(PC++);
	if (debug) std::cout<< "at addr " << int(addr) << ", " << int(valueAt(addr)) << " ";
	setValueAt(addr, ASL(valueAt(addr)));
	if (debug) std::cout<< "becomes " << int(valueAt(addr)) << "\n";
	return 5;
}
unsigned char ASL_ZPX(){
	if (debug) std::cout<< "ASL_ZPX: ";
	unsigned short addr = (valueAt(PC++) + X) % 0x100;
	if (debug) std::cout<< "at addr " << int(addr) << ", " << int(valueAt(addr)) << " ";
	setValueAt(addr, ASL(valueAt(addr)));
	if (debug) std::cout<< "becomes " << int(valueAt(addr)) << "\n";
	return 6;
}
unsigned char ASL_AB(){
	if (debug) std::cout<< "ASL_AB: ";
	unsigned short addr = valueAt(PC) + (valueAt(PC+1) << 8);
	if (debug) std::cout<< "at addr " << int(addr) << ", " << int(valueAt(addr)) << " ";
	setValueAt(addr, ASL(valueAt(addr)));
	if (debug) std::cout<< "becomes " << int(valueAt(addr)) << "\n";
	PC += 2;
	return 6;
}
unsigned char ASL_ABX(){
	if (debug) std::cout<< "ASL_ABX: ";
	unsigned short addr = valueAt(PC) + (valueAt(PC+1) << 8) + X;
	if (debug) std::cout<< "at addr " << int(addr) << ", " << int(valueAt(addr)) << " ";
	setValueAt(addr, ASL(valueAt(addr)));
	if (debug) std::cout<< "becomes " << int(valueAt(addr)) << "\n";
	PC += 2;
	return 7;
}
unsigned char ASL(unsigned char operand){
	unsigned char result = operand << 1;

	if ((operand & 0x80) == 0x80) P |= carryMask; else P &= (0xff - carryMask);
	if (result == 0) P |= zeroMask; else P &= (0xff - zeroMask);
	if ((result & 0x80) == 0x80) P |= negativeMask; else P &= (0xff - negativeMask);
	return result;
}

/******************************************************************************
* BCC - Branch if Carry Cleared
******************************************************************************/
unsigned char BCC(){
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
unsigned char BCS(){
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
unsigned char BEQ(){
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
unsigned char BIT_ZP(){
	if (debug) std::cout<< "BIT_ZP: Bit test " << int(valueAt(valueAt(PC))) << "\n";
	BIT(valueAt(valueAt(PC++)));
	return 3;
}
unsigned char BIT_AB(){
	if (debug) std::cout<< "BIT_AB: Bit test " << int(valueAt(valueAt(PC) + (valueAt(PC+1) << 8))) << "\n";
	BIT(valueAt(valueAt(PC) + (valueAt(PC+1) << 8)));
	PC += 2;
	return 4;
}
void BIT(unsigned char operand){
	if ((A & operand) == 0x00) P |= zeroMask; else P &= (0xff - zeroMask);
	if ((operand & 0x80) == 0x80) P |= negativeMask; else P &= (0xff - negativeMask);
	if ((operand & 0x40) == 0x40) P |= overflowMask; else P &= (0xff - overflowMask);
}

/******************************************************************************
* BMI - Branch if Minus
******************************************************************************/
unsigned char BMI(){
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
unsigned char BNE(){
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
unsigned char BPL(){
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
unsigned char BRK(){
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
unsigned char BVC(){
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
unsigned char BVS(){
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
unsigned char CLC(){
	if (debug) std::cout<< "CLC: " << "\n";
	P &= (0xff - carryMask);
	return 2;
}

/******************************************************************************
* CLD - Clear Decimal Flag
******************************************************************************/
unsigned char CLD(){
	if (debug) std::cout<< "CLC: " << "\n";
	P &= (0xff - decimalMask);
	return 2;
}

/******************************************************************************
* CLI - Clear Interrupt Disable Flag
******************************************************************************/
unsigned char CLI(){
	if (debug) std::cout<< "CLC: " << "\n";
	P &= (0xff - interruptDisableMask);
	return 2;
}

/******************************************************************************
* CLV - Clear Overflow Flag
******************************************************************************/
unsigned char CLV(){
	if (debug) std::cout<< "CLC: " << "\n";
	P &= (0xff - overflowMask);
	return 2;
}

/******************************************************************************
* CMP - Compare
******************************************************************************/
unsigned char CMP_IM(){
	if (debug) std::cout<< "CMP_IM: ";
	if (debug) std::cout<< "Compare " << int(valueAt(PC)) << " and " << int(A) << "\n";
	CMP(valueAt(PC++));
	return 2;
}
unsigned char CMP_ZP(){
	if (debug) std::cout<< "CMP_ZP: ";
	if (debug) std::cout<< "Compare " << int(valueAt(valueAt(PC))) << " and " << int(A) << "\n";
	CMP(valueAt(valueAt(PC++)));
	return 3;
}
unsigned char CMP_ZPX(){
	if (debug) std::cout<< "CMP_ZPX: ";
	if (debug) std::cout<< "Compare " << int(valueAt((valueAt(PC) + X) % 0x100)) << " (addr " << int((valueAt(PC) + X) % 0x100) << ") and " << int(A) << "\n";
	CMP(valueAt((valueAt(PC++) + X) % 0x100));
	return 4;
}
unsigned char CMP_AB(){
	if (debug) std::cout<< "CMP_AB: ";
	if (debug) std::cout<< "Compare " << int(valueAt(valueAt(PC) + ((valueAt(PC+1)) << 8))) << " and " << int(A) << "\n";
	CMP(valueAt(valueAt(PC) + ((valueAt(PC+1)) << 8)));
	PC += 2;
	return 4;
}
unsigned char CMP_ABX(){
	if (debug) std::cout<< "CMP_ABX: ";
	unsigned short addr = valueAt(PC) + (valueAt(PC+1) << 8);
	if (debug) std::cout<< "Compare " << int(valueAt(addr + X)) << " (addr " << int(addr) << ") and " << int(A) << "\n";
	CMP(valueAt(addr + X));
	PC += 2;
	if ((addr % 0x100) + X > 0xff) return 5;
	else return 4;
}
unsigned char CMP_ABY(){
	if (debug) std::cout<< "CMP_ABY: ";
	unsigned short addr = valueAt(PC) + (valueAt(PC+1) << 8);
	if (debug) std::cout<< "Compare " << int(valueAt(addr + Y)) << " (addr " << int(addr) << ") and " << int(A) << "\n";
	CMP(valueAt(addr + Y));
	PC += 2;
	if ((addr % 0x100) + Y > 0xff) return 5;
	else return 4;
}
unsigned char CMP_INX(){
	if (debug) std::cout<< "CMP_INX: ";
	unsigned short addr = ((valueAt(PC++) + X) % 0x100);
	addr = valueAt(addr) + (valueAt((addr+1) % 0x100) << 8);
	if (debug) std::cout<< "Compare " << int(valueAt(addr)) << " (addr " << int(addr) << ") and " << int(A) << "\n";
	CMP(valueAt(addr));
	return 6;
}
unsigned char CMP_INY(){
	if (debug) std::cout<< "CMP_INY: ";
	unsigned short addr = valueAt(valueAt(PC)) + (valueAt(valueAt(PC)+1) << 8);
	if (debug) std::cout<< "Compare " << int(valueAt(addr + Y)) << " (addr " << int(addr+Y) << ") and " << int(A) << "\n";
	CMP(valueAt(addr + Y));
	PC++;
	if ((addr % 0x100) + Y > 0xff) return 6;
	else return 5;
}
void CMP(unsigned char operand){
	unsigned char op1 = A;
	unsigned char op2 = operand;
	if (op1 >= op2) P |= carryMask; else P &= (0xff - carryMask);
	if (op1 == op2) P |= zeroMask; else P &= (0xff - zeroMask);
	if (op1 < op2) P |= negativeMask; else P &= (0xff - negativeMask);
}

/******************************************************************************
* CPX - Compare X
******************************************************************************/
unsigned char CPX_IM(){
	if (debug) std::cout<< "CPX_IM: ";
	if (debug) std::cout<< "Compare " << int(valueAt(PC)) << " and " << int(X) << "\n";
	CPX(valueAt(PC++));
	return 2;
}
unsigned char CPX_ZP(){
	if (debug) std::cout<< "CPX_ZP: ";
	if (debug) std::cout<< "Compare " << int(valueAt(valueAt(PC))) << " and " << int(X) << "\n";
	CPX(valueAt(valueAt(PC++)));
	return 3;
}
unsigned char CPX_AB(){
	if (debug) std::cout<< "CPX_AB: ";
	if (debug) std::cout<< "Compare " << int(valueAt(valueAt(PC) + ((valueAt(PC+1)) << 8))) << " and " << int(X) << "\n";
	CPX(valueAt(valueAt(PC) + ((valueAt(PC+1)) << 8)));
	PC += 2;
	return 4;
}
void CPX(unsigned char operand){
	unsigned char op1 = X;
	unsigned char op2 = operand;
	if (op1 >= op2) P |= carryMask; else P &= (0xff - carryMask);
	if (op1 == op2) P |= zeroMask; else P &= (0xff - zeroMask);
	if (op1 < op2) P |= negativeMask; else P &= (0xff - negativeMask);
}

/******************************************************************************
* CPY - Compare Y
******************************************************************************/
unsigned char CPY_IM(){
	if (debug) std::cout<< "CPY_IM: ";
	if (debug) std::cout<< "Compare " << int(valueAt(PC)) << " and " << int(Y) << "\n";
	CPY(valueAt(PC++));
	return 2;
}
unsigned char CPY_ZP(){
	if (debug) std::cout<< "CPY_ZP: ";
	if (debug) std::cout<< "Compare " << int(valueAt(valueAt(PC))) << " and " << int(Y) << "\n";
	CPY(valueAt(valueAt(PC++)));
	return 3;
}
unsigned char CPY_AB(){
	if (debug) std::cout<< "CPY_AB: ";
	if (debug) std::cout<< "Compare " << int(valueAt(valueAt(PC) + ((valueAt(PC+1)) << 8))) << " and " << int(Y) << "\n";
	CPY(valueAt(valueAt(PC) + ((valueAt(PC+1)) << 8)));
	PC += 2;
	return 4;
}
void CPY(unsigned char operand){
	unsigned char op1 = Y;
	unsigned char op2 = operand;
	if (op1 >= op2) P |= carryMask; else P &= (0xff - carryMask);
	if (op1 == op2) P |= zeroMask; else P &= (0xff - zeroMask);
	if (op1 < op2) P |= negativeMask; else P &= (0xff - negativeMask);
}

/******************************************************************************
* DCP - Decrement then Compare
******************************************************************************/
unsigned char DCP_ZP(){
	if (debug) std::cout<< "DCP_ZP: ";
	if (debug) std::cout<< "Decrement " << int(valueAt(valueAt(PC))) << " then compare to A" << "\n";
	unsigned short addr = valueAt(PC++);
	setValueAt(addr, DCP(valueAt(addr)));
	return 5;
}
unsigned char DCP_ABX(){
	if (debug) std::cout<< "DCP_ABX: ";
	unsigned short addr = valueAt(PC) + (valueAt(PC+1) << 8) + X;
	if (debug) std::cout<< "Decrement " << int(valueAt(addr)) << " then compare to A" << "\n";
	setValueAt(addr, DCP(valueAt(addr)));
	PC += 2;
	return 7;
}
unsigned char DCP_ABY(){
	if (debug) std::cout<< "DCP_ABY: ";
	unsigned short addr = valueAt(PC) + (valueAt(PC+1) << 8) + Y;
	if (debug) std::cout<< "Decrement " << int(valueAt(addr)) << " then compare to A" << "\n";
	setValueAt(addr, DCP(valueAt(addr)));
	PC += 2;
	return 7;
}
unsigned char DCP(unsigned char operand){
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
unsigned char DEC_ZP(){
	unsigned short addr = valueAt(PC++);
	setValueAt(addr, DEC(valueAt(addr)));
	return 5;
}
unsigned char DEC_ZPX(){
	unsigned short addr = (valueAt(PC++) + X) % 0x100;
	setValueAt(addr, DEC(valueAt(addr)));
	return 6;
}
unsigned char DEC_AB(){
	unsigned short addr = valueAt(PC) + (valueAt(PC+1) << 8);
	setValueAt(addr, DEC(valueAt(addr)));
	PC += 2;
	return 6;
}
unsigned char DEC_ABX(){
	unsigned short addr = valueAt(PC) + (valueAt(PC+1) << 8) + X;
	setValueAt(addr, DEC(valueAt(addr)));
	PC += 2;
	return 7;
}
unsigned char DEC(unsigned char operand){
	unsigned char result = operand - 1;

	if (result == 0) P |= zeroMask; else P &= (0xff - zeroMask);
	if ((result & 0x80) == 0x80) P |= negativeMask; else P &= (0xff - negativeMask);
	return result;
}

/******************************************************************************
* DEX - Decrement X
******************************************************************************/
unsigned char DEX(){
	unsigned char result = X - 1;

	if (result == 0) P |= zeroMask; else P &= (0xff - zeroMask);
	if ((result & 0x80) == 0x80) P |= negativeMask; else P &= (0xff - negativeMask);
	X = result;
	return 2;
}

/******************************************************************************
* DEY - Decrement Y
******************************************************************************/
unsigned char DEY(){
	unsigned char result = Y - 1;

	if (result == 0) P |= zeroMask; else P &= (0xff - zeroMask);
	if ((result & 0x80) == 0x80) P |= negativeMask; else P &= (0xff - negativeMask);
	Y = result;
	return 2;
}

/******************************************************************************
* EOR - Exclusive Or
******************************************************************************/
unsigned char EOR_IM(){
	EOR(valueAt(PC++));
	return 2;
}
unsigned char EOR_ZP(){
	EOR(valueAt(valueAt(PC++)));
	return 3;
}
unsigned char EOR_ZPX(){
	EOR(valueAt((valueAt(PC++) + X) % 0x100));
	return 4;
}
unsigned char EOR_AB(){
	EOR(valueAt(valueAt(PC) + ((valueAt(PC+1)) << 8)));
	PC += 2;
	return 4;
}
unsigned char EOR_ABX(){
	unsigned short addr = valueAt(PC) + (valueAt(PC+1) << 8);
	EOR(valueAt(addr + X));
	PC += 2;
	if ((addr % 0x100) + X > 0xff) return 5;
	else return 4;
}
unsigned char EOR_ABY(){
	unsigned short addr = valueAt(PC) + (valueAt(PC+1) << 8);
	EOR(valueAt(addr + Y));
	PC += 2;
	if ((addr % 0x100) + Y > 0xff) return 5;
	else return 4;
}
unsigned char EOR_INX(){
	unsigned short addr = ((valueAt(PC++) + X) % 0x100);
	EOR(valueAt(valueAt(addr) + (valueAt((addr+1) % 0x100) << 8)));
	return 6;
}
unsigned char EOR_INY(){
	unsigned short addr = valueAt(valueAt(PC)) + (valueAt(valueAt(PC)+1) << 8);
	EOR(valueAt(addr + Y));
	PC++;
	if ((addr % 0x100) + Y > 0xff) return 6;
	else return 5;
}
void EOR(unsigned char operand){
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
unsigned char IGN_ZP(){
	unsigned short addr = valueAt(PC++);
	IGN(addr);
	return 3;
}
unsigned char IGN_ZPX(){
	unsigned short addr = (valueAt(PC++) + X) % 0x100;
	IGN(addr);
	return 3;
}
unsigned char IGN_ABX(){
	unsigned short addr = valueAt(PC) + (valueAt(PC+1) << 8) + X;
	IGN(addr);
	PC += 2; 
	if ((addr % 0x100) + X > 0xff) return 5;
	else return 4;
}
void IGN(unsigned short operand){
	valueAt(operand);
}

/******************************************************************************
* INC - Increment
******************************************************************************/
unsigned char INC_ZP(){
	unsigned short addr = valueAt(PC++);
	setValueAt(addr, INC(valueAt(addr)));
	return 5;
}
unsigned char INC_ZPX(){
	unsigned short addr = (valueAt(PC++) + X) % 0x100;
	setValueAt(addr, INC(valueAt(addr)));
	return 6;
}
unsigned char INC_AB(){
	unsigned short addr = valueAt(PC) + (valueAt(PC+1) << 8);
	setValueAt(addr, INC(valueAt(addr)));
	PC += 2;
	return 6;
}
unsigned char INC_ABX(){
	unsigned short addr = valueAt(PC) + (valueAt(PC+1) << 8) + X;
	setValueAt(addr, INC(valueAt(addr)));
	PC += 2;
	return 7;
}
unsigned char INC(unsigned char operand){
	unsigned char incResult = operand + 1;

	if (incResult == 0) P |= zeroMask; else P &= (0xff - zeroMask);
	if ((incResult & 0x80) == 0x80) P |= negativeMask; else P &= (0xff - negativeMask);
	return incResult;
}

/******************************************************************************
* INX - Increment X
******************************************************************************/
unsigned char INX(){
	unsigned char result = X + 1;

	if (result == 0) P |= zeroMask; else P &= (0xff - zeroMask);
	if ((result & 0x80) == 0x80) P |= negativeMask; else P &= (0xff - negativeMask);
	X = result;
	return 2;
}

/******************************************************************************
* INY - Increment Y
******************************************************************************/
unsigned char INY(){
	unsigned char result = Y + 1;

	if (result == 0) P |= zeroMask; else P &= (0xff - zeroMask);
	if ((result & 0x80) == 0x80) P |= negativeMask; else P &= (0xff - negativeMask);
	Y = result;
	return 2;
}

/******************************************************************************
* ISC - Increment then Subtract
******************************************************************************/
unsigned char ISC_ZPX(){
	unsigned short addr = (valueAt(PC++) + X) % 0x100;
	setValueAt(addr, ISC(valueAt(addr)));
	return 6;
}
unsigned char ISC_AB(){
	unsigned short addr = valueAt(PC) + (valueAt(PC+1) << 8);
	setValueAt(addr, ISC(valueAt(addr)));
	PC += 2;
	return 6;
}
unsigned char ISC_ABX(){
	unsigned short addr = valueAt(PC) + (valueAt(PC+1) << 8) + X;
	setValueAt(addr, ISC(valueAt(addr)));
	PC += 2;
	return 7;
}
unsigned char ISC_ABY(){
	unsigned short addr = valueAt(PC) + (valueAt(PC+1) << 8) + Y;
	setValueAt(addr, ISC(valueAt(addr)));
	PC += 2;
	return 7;
}
unsigned char ISC_INX(){
	unsigned short addr = ((valueAt(PC++) + X) % 0x100);
	addr = valueAt(addr) + (valueAt((addr+1) % 0x100) << 8);
	setValueAt(addr, ISC(addr));
	return 8;
}
unsigned char ISC(unsigned char operand){
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
unsigned char JMP_AB(){
	unsigned short addr = valueAt(PC) + (valueAt(PC+1) << 8);
	JMP(addr);
	return 3;
}
unsigned char JMP_IN(){
	unsigned short addr = valueAt(PC) + (valueAt(PC+1) << 8);
	if ((addr & 0x00ff) == 0x00ff){
		JMP(valueAt(addr) + (valueAt(addr-0xff) << 8));
	} else{
		JMP(valueAt(addr) + (valueAt(addr+1) << 8));
	}
	return 5;
}
void JMP(unsigned short operand){
	PC = operand;
}

/******************************************************************************
* JSR - Jump to Subroutine
******************************************************************************/
unsigned char JSR(){
	unsigned short addr = valueAt(PC) + (valueAt(PC+1) << 8);
	PC += 2;
	setValueAt(0x100 + SP--, (PC & 0xff00) >> 8);
	setValueAt(0x100 + SP--, (PC & 0x00ff));
	PC = addr;
	return 6;
}

/******************************************************************************
* KIL - Kill Program
******************************************************************************/
unsigned char KIL(){
	std::cout<< "Kill Instruction " << std::hex << int(valueAt(PC-1)) << " Executed" << "\n";
	return 0;
}

/******************************************************************************
* LAX - Load Accumulator then Transfer to X
******************************************************************************/
unsigned char LAX_ABY(){
	unsigned short addr = valueAt(PC) + (valueAt(PC+1) << 8);
	LAX(valueAt(addr + Y));
	PC += 2;
	return 4;
}
void LAX(unsigned char operand){
	if (operand == 0) P |= zeroMask; else P &= (0xff - zeroMask);
	if ((operand & 0x80) == 0x80) P |= negativeMask; else P &= (0xff - negativeMask);
	A = operand;
	X = A;
}

/******************************************************************************
* LDA - Load Accumulator
******************************************************************************/
unsigned char LDA_IM(){
	LDA(valueAt(PC++));
	return 2;
}
unsigned char LDA_ZP(){
	LDA(valueAt(valueAt(PC++)));
	return 3;
}
unsigned char LDA_ZPX(){
	LDA(valueAt((valueAt(PC++) + X) % 0x100));
	return 4;
}
unsigned char LDA_AB(){
	LDA(valueAt(valueAt(PC) + ((valueAt(PC+1)) << 8)));
	PC += 2;
	return 4;
}
unsigned char LDA_ABX(){
	unsigned short addr = valueAt(PC) + (valueAt(PC+1) << 8);
	LDA(valueAt(addr + X));
	PC += 2;
	if ((addr % 0x100) + X > 0xff) return 5;
	else return 4;
}
unsigned char LDA_ABY(){
	unsigned short addr = valueAt(PC) + (valueAt(PC+1) << 8);
	LDA(valueAt(addr + Y));
	PC += 2;
	if ((addr % 0x100) + Y > 0xff) return 5;
	else return 4;
}
unsigned char LDA_INX(){
	unsigned short addr = ((valueAt(PC++) + X) % 0x100);
	LDA(valueAt(valueAt(addr) + (valueAt((addr+1) % 0x100) << 8)));
	return 6;
}
unsigned char LDA_INY(){
	unsigned short addr = valueAt(valueAt(PC)) + (valueAt(valueAt(PC)+1) << 8);
	LDA(valueAt(addr + Y));
	PC++;
	if ((addr % 0x100) + Y > 0xff) return 6;
	else return 5;
}
void LDA(unsigned char operand){
	if (operand == 0) P |= zeroMask; else P &= (0xff - zeroMask);
	if ((operand & 0x80) == 0x80) P |= negativeMask; else P &= (0xff - negativeMask);
	A = operand;
}

/******************************************************************************
* LDX - Load X
******************************************************************************/
unsigned char LDX_IM(){
	LDX(valueAt(PC++));
	return 2;
}
unsigned char LDX_ZP(){
	LDX(valueAt(valueAt(PC++)));
	return 3;
}
unsigned char LDX_ZPY(){
	LDX(valueAt((valueAt(PC++) + Y) % 0x100));
	return 4;
}
unsigned char LDX_AB(){
	LDX(valueAt(valueAt(PC) + ((valueAt(PC+1)) << 8)));
	PC += 2;
	return 4;
}
unsigned char LDX_ABY(){
	unsigned short addr = valueAt(PC) + (valueAt(PC+1) << 8);
	LDX(valueAt(addr + Y));
	PC += 2;
	if ((addr % 0x100) + Y > 0xff) return 5;
	else return 4;
}
void LDX(unsigned char operand){
	if (operand == 0) P |= zeroMask; else P &= (0xff - zeroMask);
	if ((operand & 0x80) == 0x80) P |= negativeMask; else P &= (0xff - negativeMask);
	X = operand;
}

/******************************************************************************
* LDY - Load Y
******************************************************************************/
unsigned char LDY_IM(){
	LDY(valueAt(PC++));
	return 2;
}
unsigned char LDY_ZP(){
	LDY(valueAt(valueAt(PC++)));
	return 3;
}
unsigned char LDY_ZPX(){
	LDY(valueAt((valueAt(PC++) + X) % 0x100));
	return 4;
}
unsigned char LDY_AB(){
	LDY(valueAt(valueAt(PC) + ((valueAt(PC+1)) << 8)));
	PC += 2;
	return 4;
}
unsigned char LDY_ABX(){
	unsigned short addr = valueAt(PC) + (valueAt(PC+1) << 8);
	LDY(valueAt(addr + X));
	PC += 2;
	if ((addr % 0x100) + X > 0xff) return 5;
	else return 4;
}
void LDY(unsigned char operand){
	if (operand == 0) P |= zeroMask; else P &= (0xff - zeroMask);
	if ((operand & 0x80) == 0x80) P |= negativeMask; else P &= (0xff - negativeMask);
	Y = operand;
}

/******************************************************************************
* LSR - Logical Shift Right
******************************************************************************/
unsigned char LSR_ACC(){
	A = LSR(A);
	return 2;
}
unsigned char LSR_ZP(){
	unsigned short addr = valueAt(PC++);
	setValueAt(addr, LSR(valueAt(addr)));
	return 5;
}
unsigned char LSR_ZPX(){
	unsigned short addr = (valueAt(PC++) + X) % 0x100;
	setValueAt(addr, LSR(valueAt(addr)));
	return 6;
}
unsigned char LSR_AB(){
	unsigned short addr = valueAt(PC) + (valueAt(PC+1) << 8);
	setValueAt(addr, LSR(valueAt(addr)));
	PC += 2;
	return 6;
}
unsigned char LSR_ABX(){
	unsigned short addr = valueAt(PC) + (valueAt(PC+1) << 8) + X;
	setValueAt(addr, LSR(valueAt(addr)));
	PC += 2;
	return 7;
}
unsigned char LSR(unsigned char operand){
	unsigned char result = operand >> 1;

	if ((operand & 0x01) == 0x01) P |= carryMask; else P &= (0xff - carryMask);
	if (result == 0) P |= zeroMask; else P &= (0xff - zeroMask);
	if ((result & 0x80) == 0x80) P |= negativeMask; else P &= (0xff - negativeMask);
	return result;
}

/******************************************************************************
* NOP - No Operation
******************************************************************************/
unsigned char NOP(){
	return 2;
}

/******************************************************************************
* ORA - Logical OR
******************************************************************************/
unsigned char ORA_IM(){
	ORA(valueAt(PC++));
	return 2;
}
unsigned char ORA_ZP(){
	ORA(valueAt(valueAt(PC++)));
	return 3;
}
unsigned char ORA_ZPX(){
	ORA(valueAt((valueAt(PC++) + X) % 0x100));
	return 4;
}
unsigned char ORA_AB(){
	ORA(valueAt(valueAt(PC) + ((valueAt(PC+1)) << 8)));
	PC += 2;
	return 4;
}
unsigned char ORA_ABX(){
	unsigned short addr = valueAt(PC) + (valueAt(PC+1) << 8);
	ORA(valueAt(addr + X));
	PC += 2;
	if ((addr % 0x100) + X > 0xff) return 5;
	else return 4;
}
unsigned char ORA_ABY(){
	unsigned short addr = valueAt(PC) + (valueAt(PC+1) << 8);
	ORA(valueAt(addr + Y));
	PC += 2;
	if ((addr % 0x100) + Y > 0xff) return 5;
	else return 4;
}
unsigned char ORA_INX(){
	unsigned short addr = ((valueAt(PC++) + X) % 0x100);
	ORA(valueAt(valueAt(addr) + (valueAt((addr+1) % 0x100) << 8)));
	return 6;
}
unsigned char ORA_INY(){
	unsigned short addr = valueAt(valueAt(PC)) + (valueAt(valueAt(PC)+1) << 8);
	ORA(valueAt(addr + Y));
	PC++;
	if ((addr % 0x100) + Y > 0xff) return 6;
	else return 5;
}
void ORA(unsigned char operand){
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
unsigned char PHA(){
	setValueAt(0x100 + SP--, A);
	return 3;
}

/******************************************************************************
* PHP - Push Processor Status
******************************************************************************/
unsigned char PHP(){
	setValueAt(0x100 + SP--, P);
	return 3;
}

/******************************************************************************
* PLA - Pull Accumulator
******************************************************************************/
unsigned char PLA(){
	A = valueAt(0x100 + SP++);
	return 4;
}

/******************************************************************************
* PLA - Pull Processor Status
******************************************************************************/
unsigned char PLP(){
	P = valueAt(0x100 + SP++);
	return 4;
}

/******************************************************************************
* RLA - Rotate Left then AND
******************************************************************************/
unsigned char RLA_ABX(){
	unsigned short addr = valueAt(PC) + (valueAt(PC+1) << 8) + X;
	setValueAt(addr, RLA(valueAt(addr)));
	PC += 2; 
	return 7;
}
unsigned char RLA(unsigned char operand){
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
unsigned char ROL_ACC(){
	A = ROL(A);
	return 2;
}
unsigned char ROL_ZP(){
	unsigned short addr = valueAt(PC++);
	setValueAt(addr, ROL(valueAt(addr)));
	return 5;
}
unsigned char ROL_ZPX(){
	unsigned short addr = (valueAt(PC++) + X) % 0x100;
	setValueAt(addr, ROL(valueAt(addr)));
	return 6;
}
unsigned char ROL_AB(){
	unsigned short addr = valueAt(PC) + (valueAt(PC+1) << 8);
	setValueAt(addr, ROL(valueAt(addr)));
	PC += 2;
	return 6;
}
unsigned char ROL_ABX(){
	unsigned short addr = valueAt(PC) + (valueAt(PC+1) << 8) + X;
	setValueAt(addr, ROL(valueAt(addr)));
	PC += 2;
	return 7;
}
unsigned char ROL(unsigned char operand){
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
unsigned char ROR_ACC(){
	A = ROR(A);
	return 2;
}
unsigned char ROR_ZP(){
	unsigned short addr = valueAt(PC++);
	setValueAt(addr, ROR(valueAt(addr)));
	return 5;
}
unsigned char ROR_ZPX(){
	unsigned short addr = (valueAt(PC++) + X) % 0x100;
	setValueAt(addr, ROR(valueAt(addr)));
	return 6;
}
unsigned char ROR_AB(){
	unsigned short addr = valueAt(PC) + (valueAt(PC+1) << 8);
	setValueAt(addr, ROR(valueAt(addr)));
	PC += 2;
	return 6;
}
unsigned char ROR_ABX(){
	unsigned short addr = valueAt(PC) + (valueAt(PC+1) << 8) + X;
	setValueAt(addr, ROR(valueAt(addr)));
	PC += 2;
	return 7;
}
unsigned char ROR(unsigned char operand){
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
unsigned char RTI(){
	P = valueAt(0x100 + SP++);
	PC = valueAt(0x100 + SP++);
	PC += valueAt(0x100 + SP++) << 8;
	return 6;
}

/******************************************************************************
* RTS - Return from Subroutine
******************************************************************************/
unsigned char RTS(){
	PC = valueAt(0x100 + SP++);
	PC += valueAt(0x100 + SP++) << 8;
	return 6;
}

/******************************************************************************
* SAX - Subtract With Carry
******************************************************************************/
unsigned char SAX_ZP(){
	unsigned short addr = valueAt(PC++);
	setValueAt(addr, SAX());
	return 3;
}
unsigned char SAX_INX(){
	unsigned short addr = ((valueAt(PC++) + X) % 0x100);
	setValueAt(valueAt(addr) + (valueAt((addr+1) % 0x100) << 8), SAX());
	return 6;
}
unsigned char SAX(){
	return A & X;
}

/******************************************************************************
* SBC - Subtract With Carry
******************************************************************************/
unsigned char SBC_IM(){
	SBC(valueAt(PC++));
	return 2;
}
unsigned char SBC_ZP(){
	SBC(valueAt(valueAt(PC++)));
	return 3;
}
unsigned char SBC_ZPX(){
	SBC(valueAt((valueAt(PC++) + X) % 0x100));
	return 4;
}
unsigned char SBC_AB(){
	SBC(valueAt(valueAt(PC) + ((valueAt(PC+1)) << 8)));
	PC += 2;
	return 4;
}
unsigned char SBC_ABX(){
	unsigned short addr = valueAt(PC) + (valueAt(PC+1) << 8);
	SBC(valueAt(addr + X));
	PC += 2;
	if ((addr % 0x100) + X > 0xff) return 5;
	else return 4;
}
unsigned char SBC_ABY(){
	unsigned short addr = valueAt(PC) + (valueAt(PC+1) << 8);
	SBC(valueAt(addr + Y));
	PC += 2;
	if ((addr % 0x100) + Y > 0xff) return 5;
	else return 4;
}
unsigned char SBC_INX(){
	unsigned short addr = ((valueAt(PC++) + X) % 0x100);
	SBC(valueAt(valueAt(addr) + (valueAt((addr+1) % 0x100) << 8)));
	return 6;
}
unsigned char SBC_INY(){
	unsigned short addr = valueAt(valueAt(PC)) + (valueAt(valueAt(PC)+1) << 8);
	SBC(valueAt(addr + Y));
	PC++;
	if ((addr % 0x100) + Y > 0xff) return 6;
	else return 5;
}
void SBC(unsigned char operand){
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
unsigned char SEC(){
	P |= carryMask;
	return 2;
}

/******************************************************************************
* SED - Set Decimal Flag
******************************************************************************/
unsigned char SED(){
	P |= decimalMask;
	return 2;
}

/******************************************************************************
* SEI - Set Interrupt Disable Flag
******************************************************************************/
unsigned char SEI(){
	P |= interruptDisableMask;
	return 2;
}

/******************************************************************************
* SKB - Skip Byte
******************************************************************************/
unsigned char SKB(){
	PC++;
	return 2;
}

/******************************************************************************
* SLO - Shift Left then OR
******************************************************************************/
unsigned char SLO_ZPX(){
	unsigned short addr = (valueAt(PC++) + X) % 0x100;
	setValueAt(addr, SLO(valueAt(addr)));
	return 6;
}
unsigned char SLO_AB(){
	unsigned short addr = valueAt(PC) + (valueAt(PC+1) << 8);
	setValueAt(addr, SLO(valueAt(addr)));
	PC += 2;
	return 6;
}
unsigned char SLO_ABX(){
	unsigned short addr = valueAt(PC) + (valueAt(PC+1) << 8) + X;
	setValueAt(addr, SLO(valueAt(addr)));
	PC += 2;
	return 7;
}
unsigned char SLO_INX(){
	unsigned short addr = ((valueAt(PC++) + X) % 0x100);
	addr = valueAt(addr) + (valueAt((addr+1) % 0x100) << 8);
	setValueAt(addr, SLO(valueAt(addr)));
	PC += 2;
	return 8;
}
unsigned char SLO(unsigned char operand){
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
unsigned char SRE_AB(){
	unsigned short addr = valueAt(PC) + (valueAt(PC+1) << 8);
	setValueAt(addr, SRE(valueAt(addr)));
	PC += 2;
	return 6;
}
unsigned char SRE_INY(){
	unsigned short addr = valueAt(valueAt(PC)) + (valueAt(valueAt(PC)+1) << 8);
	setValueAt(addr+Y, SRE(valueAt(addr + Y)));
	PC++;
	return 8;
}
unsigned char SRE(unsigned char operand){
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
unsigned char STA_ZP(){
	STA(valueAt(PC++));
	return 3;
}
unsigned char STA_ZPX(){
	STA((valueAt(PC++) + X) % 0x100);
	return 4;
}
unsigned char STA_AB(){
	STA(valueAt(PC) + ((valueAt(PC+1)) << 8));
	PC += 2;
	return 4;
}
unsigned char STA_ABX(){
	unsigned short addr = valueAt(PC) + (valueAt(PC+1) << 8);
	STA(addr + X);
	PC += 2;
	return 5;
}
unsigned char STA_ABY(){
	unsigned short addr = valueAt(PC) + (valueAt(PC+1) << 8);
	STA(addr + Y);
	PC += 2;
	return 5;
}
unsigned char STA_INX(){
	unsigned short addr = ((valueAt(PC++) + X) % 0x100);
	SBC(valueAt(addr) + (valueAt((addr+1) % 0x100) << 8));
	return 6;
}
unsigned char STA_INY(){
	unsigned short addr = valueAt(valueAt(PC)) + (valueAt(valueAt(PC)+1) << 8);
	SBC(addr + Y);
	PC++;
	return 6;
}
void STA(unsigned short operand){
	setValueAt(operand, A);
}

/******************************************************************************
* STX - Store X
******************************************************************************/
unsigned char STX_ZP(){
	STA(valueAt(PC++));
	return 3;
}
unsigned char STX_ZPY(){
	STA((valueAt(PC++) + Y) % 0x100);
	return 4;
}
unsigned char STX_AB(){
	STA(valueAt(PC) + ((valueAt(PC+1)) << 8));
	PC += 2;
	return 4;
}
void STX(unsigned short operand){
	setValueAt(operand, X);
}

/******************************************************************************
* STY - Store Y
******************************************************************************/
unsigned char STY_ZP(){
	STA(valueAt(PC++));
	return 3;
}
unsigned char STY_ZPX(){
	STA((valueAt(PC++) + X) % 0x100);
	return 4;
}
unsigned char STY_AB(){
	STA(valueAt(PC) + ((valueAt(PC+1)) << 8));
	PC += 2;
	return 4;
}
void STY(unsigned short operand){
	setValueAt(operand, Y);
}

/******************************************************************************
* TAX - Transfer Accumulator to X
******************************************************************************/
unsigned char TAX(){
	if (A == 0) P |= zeroMask; else P &= (0xff - zeroMask);
	if ((A & 0x80) == 0x80) P |= negativeMask; else P &= (0xff - negativeMask);
	X = A;
	return 2;
}

/******************************************************************************
* TAY - Transfer Accumulator to Y
******************************************************************************/
unsigned char TAY(){
	if (A == 0) P |= zeroMask; else P &= (0xff - zeroMask);
	if ((A & 0x80) == 0x80) P |= negativeMask; else P &= (0xff - negativeMask);
	Y = A;
	return 2;
}

/******************************************************************************
* TSX - Transfer Stack Pointer to X
******************************************************************************/
unsigned char TSX(){
	if (SP == 0) P |= zeroMask; else P &= (0xff - zeroMask);
	if ((SP & 0x80) == 0x80) P |= negativeMask; else P &= (0xff - negativeMask);
	X = SP;
	return 2;
}

/******************************************************************************
* TXA - Transfer X to Accumulator
******************************************************************************/
unsigned char TXA(){
	if (X == 0) P |= zeroMask; else P &= (0xff - zeroMask);
	if ((X & 0x80) == 0x80) P |= negativeMask; else P &= (0xff - negativeMask);
	A = X;
	return 2;
}

/******************************************************************************
* TXS - Transfer X to Stack Pointer
******************************************************************************/
unsigned char TXS(){
	if (X == 0) P |= zeroMask; else P &= (0xff - zeroMask);
	if ((X & 0x80) == 0x80) P |= negativeMask; else P &= (0xff - negativeMask);
	SP = X;
	return 2;
}

/******************************************************************************
* TYA - Transfer Y to Accumulator
******************************************************************************/
unsigned char TYA(){
	if (Y == 0) P |= zeroMask; else P &= (0xff - zeroMask);
	if ((Y & 0x80) == 0x80) P |= negativeMask; else P &= (0xff - negativeMask);
	A = Y;
	return 2;
}