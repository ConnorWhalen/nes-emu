mac:	nintendulator.cpp
	g++ -std=c++11 -stdlib=libc++ -I /usr/local/include/SDL2 -l GLEW -l SDL2 -l SDL2main -framework openGL -D MAC nintendulator.cpp execute.cpp instructions.cpp ppu.cpp view.cpp -o nintendulator.out

pi: 	nintendulator.cpp
	g++ -std=c++11 -D PI nintendulator.cpp execute.cpp instructions.cpp ppu.cpp piView.cpp -o nintendulator.out