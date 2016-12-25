#include "piView.h"

unsigned char pixels[width*height*3];

bool View::init(){
	return true;
}

void View::render(){}

bool View::event(){
	return true;
}

void View::destroy(){}