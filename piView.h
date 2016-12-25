#ifndef PIVIEW_H
#define PIVIEW_H

#include <iostream>

#include "ppu.h"

namespace View{
	bool init();
	void render();
	bool event();
	void destroy();
}

#endif