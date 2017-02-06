#include "colour.h"

unsigned char Colour::red(unsigned char colourValue){
	unsigned char ret = 0;
	switch(colourValue) {
		case 0x00: ret = 0x75;
				   break;
		case 0x01: ret = 0x27;
				   break;
		case 0x02:
		case 0x09:
		case 0x0a:
		case 0x0b:
		case 0x0d:
		case 0x0e:
		case 0x0f:
		case 0x11:
		case 0x19:
		case 0x1a:
		case 0x1b:
		case 0x1c:
		case 0x1d:
		case 0x1e:
		case 0x1f:
		case 0x2c:
		case 0x2d:
		case 0x2e:
		case 0x2f:
		case 0x3d:
		case 0x3e:
		case 0x3f: ret = 0x00;
				   break;
		case 0x03: ret = 0x47;
				   break;
		case 0x04: ret = 0x8f;
				   break;
		case 0x05: ret = 0xab;
				   break;
		case 0x06: ret = 0xa7;
				   break;
		case 0x07: ret = 0x7f;
				   break;
		case 0x08: ret = 0x43;
				   break;
		case 0x0c: ret = 0x1b;
				   break;
		case 0x10: ret = 0xbc;
				   break;
		case 0x12: ret = 0x23;
				   break;
		case 0x13: ret = 0x83;
				   break;
		case 0x14: ret = 0xbf;
				   break;
		case 0x15: ret = 0xe7;
				   break;
		case 0x16: ret = 0xdb;
				   break;
		case 0x17: ret = 0xcb;
				   break;
		case 0x18: ret = 0x8b;
				   break;
		case 0x20:
		case 0x25:
		case 0x26:
		case 0x27:
		case 0x30:
		case 0x34:
		case 0x35:
		case 0x36:
		case 0x37:
		case 0x38: ret = 0xff;
				   break;
		case 0x21: ret = 0x3f;
				   break;
		case 0x22: ret = 0x5f;
				   break;
		case 0x23: ret = 0xa7;
				   break;
		case 0x24: ret = 0xf7;
				   break;
		case 0x28: ret = 0xf3;
				   break;
		case 0x29: ret = 0x83;
				   break;
		case 0x2a: ret = 0x4f;
				   break;
		case 0x2b: ret = 0x58;
				   break;
		case 0x31: ret = 0xab;
				   break;
		case 0x32: ret = 0xc7;
				   break;
		case 0x33: ret = 0xd7;
				   break;
		case 0x39: ret = 0xe3;
				   break;
		case 0x3a: ret = 0xab;
				   break;
		case 0x3b: ret = 0xb3;
				   break;
		case 0x3c: ret = 0x9f;
				   break;
		default:   std::cout << "Colour " << std::hex << int(colourValue) << " not recognized." << "\n";
				   break;
	}
	return ret;
}

unsigned char Colour::green(unsigned char colourValue){
	unsigned char ret = 0;
	switch(colourValue) {
		case 0x00: ret = 0x75;
				   break;
		case 0x01: ret = 0x1b;
				   break;
		case 0x02:
		case 0x03:
		case 0x04:
		case 0x05:
		case 0x06:
		case 0x0d:
		case 0x0e:
		case 0x0f:
		case 0x13:
		case 0x14:
		case 0x15:
		case 0x1d:
		case 0x1e:
		case 0x1f:
		case 0x2d:
		case 0x2e:
		case 0x2f:
		case 0x3d:
		case 0x3e:
		case 0x3f: ret = 0x00;
				   break;
		case 0x07: ret = 0x0b;
				   break;
		case 0x08: ret = 0x2f;
				   break;
		case 0x09: ret = 0x47;
				   break;
		case 0x0a: ret = 0x51;
				   break;
		case 0x0b:
		case 0x0c: ret = 0x3f;
				   break;
		case 0x10: ret = 0xbc;
				   break;
		case 0x11: ret = 0x73;
				   break;
		case 0x12: ret = 0x3b;
				   break;
		case 0x16: ret = 0x2b;
				   break;
		case 0x17: ret = 0x4f;
				   break;
		case 0x18: ret = 0x73;
				   break;
		case 0x19: ret = 0x97;
				   break;
		case 0x1a: ret = 0xab;
				   break;
		case 0x1b: ret = 0x93;
				   break;
		case 0x1c: ret = 0x83;
				   break;
		case 0x20:
		case 0x30:
		case 0x39:
		case 0x3b:
		case 0x3c: ret = 0xff;
				   break;
		case 0x21: ret = 0xbf;
				   break;
		case 0x22: ret = 0x97;
				   break;
		case 0x23: ret = 0x8b;
				   break;
		case 0x24: ret = 0x7b;
				   break;
		case 0x25:
		case 0x26: ret = 0x77;
				   break;
		case 0x27: ret = 0x9b;
				   break;
		case 0x28: ret = 0xbf;
				   break;
		case 0x29: ret = 0xd3;
				   break;
		case 0x2a: ret = 0xdf;
				   break;
		case 0x2b: ret = 0xf8;
				   break;
		case 0x2c: ret = 0xeb;
				   break;
		case 0x31: ret = 0xe7;
				   break;
		case 0x32: ret = 0xd7;
				   break;
		case 0x33: ret = 0xf8;
				   break;
		case 0x34:
		case 0x35: ret = 0xc7;
				   break;
		case 0x36: ret = 0xbf;
				   break;
		case 0x37: ret = 0xdb;
				   break;
		case 0x38: ret = 0xe7;
				   break;
		case 0x3a: ret = 0xf3;
				   break;
		default:   std::cout << "Colour " << std::hex << int(colourValue) << " not recognized." << "\n";
				   break;
	}
	return ret;
}

unsigned char Colour::blue(unsigned char colourValue){
	unsigned char ret = 0;
	switch(colourValue) {
		case 0x00: ret = 0x75;
				   break;
		case 0x01: ret = 0x8f;
				   break;
		case 0x02: ret = 0xab;
				   break;
		case 0x03: ret = 0x9f;
				   break;
		case 0x04: ret = 0x77;
				   break;
		case 0x05: ret = 0x13;
				   break;
		case 0x06:
		case 0x07:
		case 0x08:
		case 0x09:
		case 0x0a:
		case 0x0d:
		case 0x0e:
		case 0x0f:
		case 0x16:
		case 0x18:
		case 0x19:
		case 0x1a:
		case 0x1d:
		case 0x1e:
		case 0x1f:
		case 0x2d:
		case 0x2e:
		case 0x2f:
		case 0x3d:
		case 0x3e:
		case 0x3f: ret = 0x00;
				   break;
		case 0x0b: ret = 0x17;
				   break;
		case 0x0c: ret = 0x5f;
				   break;
		case 0x10: ret = 0xbc;
				   break;
		case 0x11:
		case 0x12: ret = 0xef;
				   break;
		case 0x13: ret = 0xf3;
				   break;
		case 0x14: ret = 0xbf;
				   break;
		case 0x15: ret = 0x5b;
				   break;
		case 0x17: ret = 0x0f;
				   break;
		case 0x1b: ret = 0x3b;
				   break;
		case 0x1c: ret = 0x8b;
				   break;
		case 0x20:
		case 0x21:
		case 0x22:
		case 0x24:
		case 0x30:
		case 0x31:
		case 0x32:
		case 0x33:
		case 0x34:
		case 0x23: ret = 0xfd;
				   break;
		case 0x25: ret = 0xb7;
				   break;
		case 0x26: ret = 0x63;
				   break;
		case 0x27: ret = 0x3b;
				   break;
		case 0x28: ret = 0x3f;
				   break;
		case 0x29: ret = 0x13;
				   break;
		case 0x2a: ret = 0x4b;
				   break;
		case 0x2b: ret = 0x98;
				   break;
		case 0x2c:
		case 0x35: ret = 0xdb;
				   break;
		case 0x36: ret = 0xb3;
				   break;
		case 0x37: ret = 0xab;
				   break;
		case 0x38:
		case 0x39: ret = 0xa3;
				   break;
		case 0x3a: ret = 0xbf;
				   break;
		case 0x3b: ret = 0xcf;
				   break;
		case 0x3c: ret = 0xf3;
				   break;
		default:   std::cout << "Colour " << std::hex << int(colourValue) << " not recognized." << "\n";
				   break;
	}
	return ret;
}