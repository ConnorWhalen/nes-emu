#include "view.h"

const GLchar* vertexSource =
    "#version 150 core\n"
    "in vec2 position;"
    "in vec2 texCoord;"
    "out vec2 TexCoord;"
    "uniform mat4 proj;"
    "void main()"
    "{"
    "    TexCoord = texCoord;"
    "    gl_Position = vec4(position, 0.0, 1.0);"
    "}";
const GLchar* fragmentSource =
    "#version 150 core\n"
    "in vec2 TexCoord;"
    "out vec4 outColor;"
    "uniform sampler2D tex;"
    "void main()"
    "{"
    "    outColor = texture(tex, TexCoord);"
    "}";

View::View(PPU* ppu){
	this->ppu = ppu;
}

bool View::init(){
	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

	window = SDL_CreateWindow("OpenGL", 100, 100, width, height, SDL_WINDOW_OPENGL);
	context = SDL_GL_CreateContext(window);
	SDL_GL_MakeCurrent(window, context);

	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if(err!=GLEW_OK) {
		// Problem: glewInit failed, something is seriously wrong.
		std::cout<<"glewInit failed: "<<glewGetErrorString(err)<<"\n";
		return false;
	}

	// Preliminary error check, should absorb a GL_INVALID_ENUM (1) that happens for no reason
	// GLenum e = glGetError();
	// if (e==GL_NO_ERROR)
	// 	std::cout<<"noError";
	// if (e==GL_INVALID_ENUM)
	// 	std::cout<<"1";
	// else if (e==GL_INVALID_VALUE)
	// 	std::cout<<"2";
	// else if (e==GL_INVALID_OPERATION)
	// 	std::cout<<"3";
	// else if (e==GL_INVALID_FRAMEBUFFER_OPERATION)
	// 	std::cout<<"4";
	// else if (e==GL_OUT_OF_MEMORY)
	// 	std::cout<<"5";
	// else if (e==GL_STACK_UNDERFLOW)
	// 	std::cout<<"6";
	// else if (e==GL_STACK_OVERFLOW)
	// 	std::cout<<"7";
	// std::cout<<"\n";

	GLfloat vertices[] = {
		//X     Y     TX    TY
		-1.0f, -1.0f, 0.0f, 1.0f,
		 1.0f, -1.0f, 1.0f, 1.0f,
		 1.0f,  1.0f, 1.0f, 0.0f,
		 1.0f,  1.0f, 1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f, 0.0f,
		-1.0f, -1.0f, 0.0f, 1.0f
	};

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenTextures(1, textures);

	glGenBuffers(1, &vbo); // Generate 1 buffer
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);

	GLint status;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
	if (status!=GL_TRUE){
		std::cout<<"vertex shader error\n";
		char buffer[512];
		glGetShaderInfoLog(vertexShader, 512, NULL, buffer);
		for (int i = 0; i < 512; i++){
			std::cout<<buffer[i];
		}
		std::cout<<"\n\n";
		return false;
	}

	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
	if (status!=GL_TRUE){
		std::cout<<"fragment shader error\n";
		char buffer[512];
		glGetShaderInfoLog(fragmentShader, 512, NULL, buffer);
		for (int i = 0; i < 512; i++){
			std::cout<<buffer[i];
		}
		std::cout<<"\n\n";
		return false;
	}

	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glBindFragDataLocation(shaderProgram, 0, "outColor");
	glLinkProgram(shaderProgram);
	glUseProgram(shaderProgram);

	GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), 0);

	GLint texAttrib = glGetAttribLocation(shaderProgram, "texCoord");
	glEnableVertexAttribArray(texAttrib);
	glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(2*sizeof(float)));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, ppu->getPixels());
	glUniform1i(glGetUniformLocation(shaderProgram, "tex"), 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	return true;
}

bool View::event(){
	if (SDL_PollEvent(&inputEvent))
	{
		if (inputEvent.type == SDL_QUIT) return false;
		else if (inputEvent.type == SDL_KEYUP && inputEvent.key.keysym.sym == SDLK_ESCAPE) return false;
	}
	return true;
}

void View::render(){
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, ppu->getPixels());
	glDrawArrays(GL_TRIANGLES, 0, 6);

	SDL_GL_SwapWindow(window);
}

void View::destroy(){
	glDeleteTextures(1, textures);
	glDeleteProgram(shaderProgram);
	glDeleteShader(fragmentShader);
	glDeleteShader(vertexShader);
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
	SDL_GL_DeleteContext(context);
	SDL_Quit();
}