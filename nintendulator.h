#ifndef NINTENDULATOR_H
#define NINTENDULATOR_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <thread>

#include "cpu.h"
#include "ppu.h"
#include "controller.h"
#ifdef MAC
	#include "view.h"
#endif
#ifdef PI
	#include "piView.h"
#endif

extern bool debug;
extern bool test;

#endif