#ifndef VIEW_H
#define VIEW_H

#define GLEW_STATIC
#include <GL/glew.h>
#include <SDL.h>
#include <iostream>

#include "ppu.h"

class View {
	public:
		View(PPU* ppu);
		bool init();
		void render();
		bool event();
		void destroy();
	private:
		PPU* ppu;

		SDL_Event inputEvent;
		SDL_Window* window;
		SDL_GLContext context;
		GLuint vao;
		GLuint vbo;
		GLuint textures[1];
		GLuint vertexShader;
		GLuint fragmentShader;
		GLuint shaderProgram;
};

#endif