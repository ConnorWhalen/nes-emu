mac:	nintendulator.cpp
	g++ -std=c++11 -stdlib=libc++ -I /usr/local/include/SDL2 -l portaudio -l GLEW -l SDL2 -l SDL2main -framework openGL -D MAC nintendulator.cpp cpu.cpp ppu.cpp apu.cpp colour.cpp controller.cpp view.cpp audio.cpp -o nintendulator.out

pi: 	nintendulator.cpp
	g++ -std=c++11 -I /opt/vc/include -I /opt/vc/include/interface/vmcs_host/linux -I /opt/vc/include/interface/vcos/pthreads -l shapes -D PI nintendulator.cpp cpu.cpp ppu.cpp apu.cpp colour.cpp controller.cpp piView.cpp audio.cpp -o nintendulator.out
