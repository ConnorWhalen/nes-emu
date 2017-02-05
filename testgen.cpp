#include <fstream>
#include <iostream>

int main(){
	std::ofstream out;
	out.open("roms/test.nes", std::ios_base::trunc);
	for (int i = 0; i < 4; i++) out.put(0); // NES header
	out.put(2); // rom page count
	out.put(0); // CHROM page count
	out.put(0); // flags 1
	out.put(0); // flags 2
	for (int i = 0; i < 8; i++) out.put(0); // fill out rest of header

	out.put(0xa9); // LDA IM
	out.put(0x11);
	out.put(0x85); // STA IM
	out.put(0x00);

	out.put(0x69); // ADC IM
	out.put(0x11);
	out.put(0x85); // STA IM
	out.put(0x01);

	out.put(0x65); // ADC ZP
	out.put(0x00);
	out.put(0x85); // STA IM
	out.put(0x02);

	out.put(0xa2); // LDX IM
	out.put(0x01);
	out.put(0x75); // ADC ZPX
	out.put(0x02);
	out.put(0x85); // STA IM
	out.put(0x03);

	out.put(0xa9); // LDA IM
	out.put(0x11);
	out.put(0x8d); // STA AB
	out.put(0x00);
	out.put(0x01);
	out.put(0x6d); // ADC AB
	out.put(0x00);
	out.put(0x01);
	out.put(0x85); // STA IM
	out.put(0x04);

	out.put(0xa9); // LDA IM
	out.put(0x11);
	out.put(0x7d); // ADC ABX
	out.put(0x00);
	out.put(0x01);
	out.put(0x85); // STA IM
	out.put(0x05);

	out.put(0x79); // ADC ABY
	out.put(0x00);
	out.put(0x01);
	out.put(0x85); // STA IM
	out.put(0x06);

	out.put(0xa9); // LDA IM
	out.put(0x11);
	out.put(0x8d); // STA AB
	out.put(0x22);
	out.put(0x00);
	out.put(0x61); // ADC INX
	out.put(0x05);
	out.put(0x85); // STA IM
	out.put(0x07);

	out.put(0xa0); // LDY IM
	out.put(0x01);
	out.put(0x8d); // STA AB
	out.put(0x23);
	out.put(0x00);
	out.put(0x71); // ADC INY
	out.put(0x07);
	out.put(0x85); // STA IM
	out.put(0x08);

	out.put(0x62); // KIL

	for (int i = 0; i < 16384*2-63; i++) out.put(0); // fill out ROM

	out.put(0x00);
	out.put(0x80); // reset PC to start of ROM
	out.put(0);
	out.put(0); // fill out ROM

	out.close();
	return 0;
}