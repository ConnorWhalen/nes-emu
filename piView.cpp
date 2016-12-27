#include "piView.h"

unsigned char pixels[width*height*3];

bool View::init(){
	initWindowSize(100, 100, width, height);
	return true;
}

void View::render(){
	Start(width, height);
	End();
}

bool View::event(){
	return true;
}

void View::destroy(){
	finish();
}