default:
	g++ -g -o build/game.exe src/*.cpp src/song/*.cpp src/pak/*.cpp -O2 -fdiagnostics-color=always -Wall -pedantic -static -I inc/ -L lib/ -lraylib -lopengl32 -lgdi32 -lwinmm