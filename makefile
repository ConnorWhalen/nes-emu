mac:		nintendulator.cpp
	g++ -std=c++11 -stdlib=libc++ -I /usr/local/include/SDL2 -l portaudio -l GLEW -l SDL2 -l SDL2main -framework openGL -D MAC nintendulator.cpp cpu.cpp ppu.cpp apu.cpp colour.cpp controller.cpp view.cpp audio.cpp -o nintendulator.out

pi: 		nintendulator.cpp
	g++ -std=c++11 -I /opt/vc/include -I /opt/vc/include/interface/vmcs_host/linux -I /opt/vc/include/interface/vcos/pthreads -l shapes -D PI nintendulator.cpp cpu.cpp ppu.cpp apu.cpp colour.cpp controller.cpp piView.cpp audio.cpp -o nintendulator.out

testCPU:	testCPU.cpp
	g++ -std=c++11 -stdlib=libc++ testCPU.cpp cpu.cpp test/fakeController.cpp test/fakeAPU.cpp test/fakePPU.cpp -o testCPU.out

testPPU:	testPPU.cpp
	g++ -std=c++11 -stdlib=libc++ testPPU.cpp ppu.cpp -o testPPU.out