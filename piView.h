#ifndef PIVIEW_H
#define PIVIEW_H

#include <iostream>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include "fontinfo.h"
#include "shapes.h"

#include "ppu.h"

namespace View{
	bool init();
	void render();
	bool event();
	void destroy();
}

#endif