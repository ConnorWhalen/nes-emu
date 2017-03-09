#ifndef VIEW_H
#define VIEW_H

#define GLEW_STATIC
#include <GL/glew.h>
#include <SDL.h>
#include <iostream>

#include "ppu.h"
#include "controller.h"

class View {
	public:
		View(PPU* ppu, Controller* player1, Controller* player2);
		bool init();
		void render();
		bool event();
		void destroy();
		bool quit();
	private:
		PPU* ppu;
		Controller* player1;
		Controller* player2;

		bool quitFlag;

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