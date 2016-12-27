mac:	nintendulator.cpp
	g++ -std=c++11 -stdlib=libc++ -I /usr/local/include/SDL2 -l GLEW -l SDL2 -l SDL2main -framework openGL -D MAC nintendulator.cpp execute.cpp instructions.cpp ppu.cpp view.cpp -o nintendulator.out

pi: 	nintendulator.cpp
	g++ -std=c++11 -I /opt/vc/include -I /opt/vc/include/interface/vmcs_host/linux -I /opt/vc/include/interface/vcos/pthreads -l shapes -D PI nintendulator.cpp execute.cpp instructions.cpp ppu.cpp piView.cpp -o nintendulator.out
