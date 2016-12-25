#ifndef VIEW_H
#define VIEW_H

#define GLEW_STATIC
#include <GL/glew.h>
#include <SDL.h>
#include <iostream>

#include "ppu.h"

namespace View{
	bool init();
	void render();
	bool event();
	void destroy();
}

#endif