#include "controller.h"

Controller::Controller(bool isPlayer1){
		a = false;
		b = false;
		select = false;
		start = false;
		up = false;
		down = false;
		left = false;
		right = false;
		reset = false;
		readCounter = 0;
		player1 = isPlayer1;
}

unsigned char Controller::read(){
	if (reset) readCounter = 0;
	unsigned char ret = 0;
	switch (readCounter) {
		case 0: ret = a ? 0x41 : 0x40;
				break;
		case 1: ret = b ? 0x41 : 0x40;
				break;
		case 2: ret = select ? 0x41 : 0x40;
				break;
		case 3: ret = start ? 0x41 : 0x40;
				break;
		case 4: ret = up ? 0x41 : 0x40;
				break;
		case 5: ret = down ? 0x41 : 0x40;
				break;
		case 6: ret = left ? 0x41 : 0x40;
				break;
		case 7: ret = right ? 0x41 : 0x40;
				break;
		case 19: ret = player1 ? 0x40 : 0x41;
		case 20: ret = player1 ? 0x41 : 0x40;
				break;
		default: ret = 0x40;
				break;
	}
	readCounter = (readCounter + 1) % 24;
	return ret;
}

void Controller::write(bool value){
	reset = value;
}

void Controller::setA(bool value){
	a = value;
}

void Controller::setB(bool value){
	b = value;
}

void Controller::setSelect(bool value){
	select = value;
}

void Controller::setStart(bool value){
	start = value;
}

void Controller::setUp(bool value){
	up = value;
}

void Controller::setDown(bool value){
	down = value;
}

void Controller::setLeft(bool value){
	left = value;
}

void Controller::setRight(bool value){
	right = value;
}